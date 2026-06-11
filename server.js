const express = require('express');
const cors = require('cors');
const bodyParser = require('body-parser');
const { exec } = require('child_process');
const { promisify } = require('util');
const path = require('path');
const mongoose = require('mongoose');

const execAsync = promisify(exec);

const app = express();
const PORT = Number.parseInt(process.env.PORT, 10) || 3000;
const HOST = process.env.HOST || '0.0.0.0';

// MongoDB Connection
const MONGODB_URI = process.env.MONGODB_URI || 'mongodb://localhost:27017/sorting-analyzer';

mongoose.connect(MONGODB_URI, {
  useNewUrlParser: true,
  useUnifiedTopology: true,
}).then(() => {
  console.log('✓ MongoDB connected');
}).catch((err) => {
  console.log('⚠ MongoDB connection warning:', err.message);
  console.log('  Note: Data storage will be disabled. Install MongoDB to enable history.');
});

// MongoDB Schema for storing sorting interactions
const sortingHistorySchema = new mongoose.Schema({
  timestamp: { type: Date, default: Date.now, index: true },
  language: String,
  algorithm: String,
  inputElements: [Number],
  outputElements: [Number],
  actualTimeMs: Number,
  comparisons: Number,
  swaps: Number,
  isSorted: Boolean,
});

const SortingHistory = mongoose.model('SortingHistory', sortingHistorySchema);

// Helper function to log interaction
async function logInteraction(language, algorithm, input, output) {
  try {
    if (mongoose.connection.readyState === 1) {
      const history = new SortingHistory({
        language,
        algorithm,
        inputElements: input.input || input,
        outputElements: output.output || output.output,
        actualTimeMs: output.actual_time_ms,
        comparisons: output.comparisons,
        swaps: output.swaps,
        isSorted: output.is_sorted,
      });
      await history.save();
    }
  } catch (err) {
    console.log('⚠ Failed to log interaction:', err.message);
  }
}

// Middleware
app.use(cors());
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true }));

// Base path for executables
const DIST_SORTING_PATH = path.join(__dirname, 'dist-sorting');

// Language executables mapping
// All languages use pre-compiled .exe, Java uses java -jar
const LANGUAGES = {
  c: { exe: path.join(DIST_SORTING_PATH, 'c', 'SortingC.exe') },
  cpp: { exe: path.join(DIST_SORTING_PATH, 'cpp', 'SortingCpp.exe') },
  python: { exe: path.join(DIST_SORTING_PATH, 'python', 'SortingPython.exe') },
  go: { exe: path.join(DIST_SORTING_PATH, 'go', 'SortingGo.exe') },
  rust: { exe: path.join(DIST_SORTING_PATH, 'rust', 'SortingRust.exe') },
  csharp: { exe: path.join(DIST_SORTING_PATH, 'csharp', 'SortingCSharp.exe') },
  java: { jar: path.join(DIST_SORTING_PATH, 'java', 'SortingJava.jar') },
};

// Valid algorithms
const VALID_ALGORITHMS = [
  'bubble', 'selection', 'insertion', 'merge', 'quick', 'heap',
  'counting', 'radix', 'bucket', 'tim', 'intro'
];

// Input validation function
function validateElements(elements) {
  if (!Array.isArray(elements)) {
    throw new Error('Elements must be an array');
  }

  if (elements.length > 50) {
    throw new Error('Maximum 50 elements allowed');
  }

  for (let i = 0; i < elements.length; i++) {
    const val = parseFloat(elements[i]);
    if (isNaN(val)) {
      throw new Error(`Element at index ${i} is not a valid number: ${elements[i]}`);
    }
    if (val < 0 || val > 1000) {
      throw new Error(`Element at index ${i} value ${val} is out of range (0-1000)`);
    }
  }

  return true;
}

// Helper function to execute sorting algorithm (async)
async function executeSorting(language, algorithm, elements = []) {
  const langConfig = LANGUAGES[language];
  
  if (!langConfig) {
    throw new Error(`Language not found: ${language}`);
  }

  if (!VALID_ALGORITHMS.includes(algorithm)) {
    throw new Error(`Invalid algorithm: ${algorithm}`);
  }

  // Validate input elements if provided
  if (elements && elements.length > 0) {
    validateElements(elements);
  }

  // Build command: use java -jar for Java, exe for everything else
  let cmd;
  if (langConfig.jar) {
    cmd = `java -jar "${langConfig.jar}" "${algorithm}"`;
  } else {
    cmd = `"${langConfig.exe}" "${algorithm}"`;
  }
  
  if (elements && elements.length > 0) {
    cmd += ' ' + elements.map(e => parseFloat(e)).join(' ');
  }

  try {
    const { stdout } = await execAsync(cmd, { 
      encoding: 'utf-8',
      maxBuffer: 10 * 1024 * 1024, // 10MB buffer
      timeout: 30000 // 30 second timeout per request
    });
    
    return JSON.parse(stdout.trim());
  } catch (error) {
    throw new Error(`Execution error: ${error.message}`);
  }
}

// Routes

// Health check
app.get('/api/health', (req, res) => {
  res.json({ status: 'ok', message: 'Sorting Algorithm Backend is running' });
});

// List available languages
app.get('/api/languages', (req, res) => {
  res.json({
    languages: Object.keys(LANGUAGES),
    algorithms: VALID_ALGORITHMS
  });
});

// Sort with specific language and algorithm
app.post('/api/sort/:language/:algorithm', async (req, res) => {
  try {
    const { language, algorithm } = req.params;
    const { elements } = req.body;

    const result = await executeSorting(language, algorithm, elements);
    
    // Log interaction to database
    await logInteraction(language, algorithm, result, result);
    
    res.json({
      success: true,
      language,
      data: result
    });
  } catch (error) {
    res.status(400).json({
      success: false,
      error: error.message
    });
  }
});

// Sort with query parameters (for GET requests)
app.get('/api/sort/:language/:algorithm', async (req, res) => {
  try {
    const { language, algorithm } = req.params;
    const elements = req.query.elements ? 
      String(req.query.elements).split(',').map(e => e.trim()) : [];

    const result = await executeSorting(language, algorithm, elements);
    
    // Log interaction to database
    await logInteraction(language, algorithm, result, result);
    
    res.json({
      success: true,
      language,
      data: result
    });
  } catch (error) {
    res.status(400).json({
      success: false,
      error: error.message
    });
  }
});

// Compare performance across all languages
app.post('/api/compare/:algorithm', async (req, res) => {
  try {
    const { algorithm } = req.params;
    const { elements } = req.body;

    if (!VALID_ALGORITHMS.includes(algorithm)) {
      throw new Error(`Invalid algorithm: ${algorithm}`);
    }

    // Execute all languages in parallel for faster comparison
    const langPromises = Object.entries(LANGUAGES).map(async ([lang, exe]) => {
      try {
        const result = await executeSorting(lang, algorithm, elements);
        return [lang, result];
      } catch (err) {
        return [lang, { error: err.message }];
      }
    });

    const langResults = await Promise.all(langPromises);
    const results = Object.fromEntries(langResults);

    res.json({
      success: true,
      algorithm,
      results
    });
  } catch (error) {
    res.status(400).json({
      success: false,
      error: error.message
    });
  }
});

// Compare all algorithms for a language
app.post('/api/algorithms/:language', async (req, res) => {
  try {
    const { language } = req.params;
    const { elements } = req.body;

    if (!LANGUAGES[language]) {
      throw new Error(`Language not found: ${language}`);
    }

    // Execute all algorithms in parallel for faster testing
    const algoPromises = VALID_ALGORITHMS.map(async (algo) => {
      try {
        const result = await executeSorting(language, algo, elements);
        return [algo, result];
      } catch (err) {
        return [algo, { error: err.message }];
      }
    });

    const algoResults = await Promise.all(algoPromises);
    const results = Object.fromEntries(algoResults);

    res.json({
      success: true,
      language,
      results
    });
  } catch (error) {
    res.status(400).json({
      success: false,
      error: error.message
    });
  }
});

// Get sorting history - all records
app.get('/api/history', async (req, res) => {
  try {
    if (mongoose.connection.readyState !== 1) {
      return res.status(503).json({
        success: false,
        error: 'Database not connected'
      });
    }

    const limit = Math.min(parseInt(req.query.limit) || 100, 1000);
    const page = parseInt(req.query.page) || 1;
    const skip = (page - 1) * limit;

    const records = await SortingHistory.find()
      .sort({ timestamp: -1 })
      .limit(limit)
      .skip(skip)
      .exec();

    const total = await SortingHistory.countDocuments();

    res.json({
      success: true,
      data: records,
      pagination: {
        page,
        limit,
        total,
        pages: Math.ceil(total / limit)
      }
    });
  } catch (error) {
    res.status(400).json({
      success: false,
      error: error.message
    });
  }
});

// Get history for specific language
app.get('/api/history/language/:language', async (req, res) => {
  try {
    if (mongoose.connection.readyState !== 1) {
      return res.status(503).json({
        success: false,
        error: 'Database not connected'
      });
    }

    const { language } = req.params;
    const limit = Math.min(parseInt(req.query.limit) || 50, 500);
    const page = parseInt(req.query.page) || 1;
    const skip = (page - 1) * limit;

    const records = await SortingHistory.find({ language })
      .sort({ timestamp: -1 })
      .limit(limit)
      .skip(skip)
      .exec();

    const total = await SortingHistory.countDocuments({ language });

    res.json({
      success: true,
      language,
      data: records,
      pagination: {
        page,
        limit,
        total,
        pages: Math.ceil(total / limit)
      }
    });
  } catch (error) {
    res.status(400).json({
      success: false,
      error: error.message
    });
  }
});

// Get history for specific algorithm
app.get('/api/history/algorithm/:algorithm', async (req, res) => {
  try {
    if (mongoose.connection.readyState !== 1) {
      return res.status(503).json({
        success: false,
        error: 'Database not connected'
      });
    }

    const { algorithm } = req.params;
    const limit = Math.min(parseInt(req.query.limit) || 50, 500);
    const page = parseInt(req.query.page) || 1;
    const skip = (page - 1) * limit;

    const records = await SortingHistory.find({ algorithm })
      .sort({ timestamp: -1 })
      .limit(limit)
      .skip(skip)
      .exec();

    const total = await SortingHistory.countDocuments({ algorithm });

    res.json({
      success: true,
      algorithm,
      data: records,
      pagination: {
        page,
        limit,
        total,
        pages: Math.ceil(total / limit)
      }
    });
  } catch (error) {
    res.status(400).json({
      success: false,
      error: error.message
    });
  }
});

// Get history for specific language and algorithm
app.get('/api/history/:language/:algorithm', async (req, res) => {
  try {
    if (mongoose.connection.readyState !== 1) {
      return res.status(503).json({
        success: false,
        error: 'Database not connected'
      });
    }

    const { language, algorithm } = req.params;
    const limit = Math.min(parseInt(req.query.limit) || 50, 500);
    const page = parseInt(req.query.page) || 1;
    const skip = (page - 1) * limit;

    const records = await SortingHistory.find({ language, algorithm })
      .sort({ timestamp: -1 })
      .limit(limit)
      .skip(skip)
      .exec();

    const total = await SortingHistory.countDocuments({ language, algorithm });

    res.json({
      success: true,
      language,
      algorithm,
      data: records,
      pagination: {
        page,
        limit,
        total,
        pages: Math.ceil(total / limit)
      }
    });
  } catch (error) {
    res.status(400).json({
      success: false,
      error: error.message
    });
  }
});

// Get database statistics
app.get('/api/history/stats', async (req, res) => {
  try {
    if (mongoose.connection.readyState !== 1) {
      return res.status(503).json({
        success: false,
        error: 'Database not connected'
      });
    }

    const total = await SortingHistory.countDocuments();
    const byLanguage = await SortingHistory.aggregate([
      { $group: { _id: '$language', count: { $sum: 1 } } }
    ]);
    const byAlgorithm = await SortingHistory.aggregate([
      { $group: { _id: '$algorithm', count: { $sum: 1 } } }
    ]);

    res.json({
      success: true,
      totalRecords: total,
      byLanguage: Object.fromEntries(byLanguage.map(l => [l._id, l.count])),
      byAlgorithm: Object.fromEntries(byAlgorithm.map(a => [a._id, a.count]))
    });
  } catch (error) {
    res.status(400).json({
      success: false,
      error: error.message
    });
  }
});

// 404 handler
app.use((req, res) => {
  res.status(404).json({
    success: false,
    error: 'Endpoint not found'
  });
});

// Start server
app.listen(PORT, HOST, () => {
  console.log(`🚀 Sorting Algorithm Backend running on http://${HOST}:${PORT}`);
  console.log(`Available languages: ${Object.keys(LANGUAGES).join(', ')}`);
  console.log(`Available algorithms: ${VALID_ALGORITHMS.join(', ')}`);
});
