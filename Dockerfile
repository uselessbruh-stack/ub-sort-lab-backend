# Stage 1: Build C and C++ binaries
FROM gcc:12 AS cpp-builder
WORKDIR /app
COPY dist-sorting/c/sorting.c ./c/
COPY dist-sorting/cpp/main.cpp ./cpp/
RUN mkdir -p /app/out/c /app/out/cpp && \
    gcc -O3 ./c/sorting.c -o /app/out/c/SortingC && \
    g++ -O3 ./cpp/main.cpp -o /app/out/cpp/SortingCpp

# Stage 2: Build Go binary
FROM golang:1.21 AS go-builder
WORKDIR /app
COPY dist-sorting/go/ ./go/
RUN mkdir -p /app/out/go && \
    cd ./go && \
    go build -o /app/out/go/SortingGo main.go

# Stage 3: Build Rust binary
FROM rust:1.75 AS rust-builder
WORKDIR /app
COPY dist-sorting/rust/ ./rust/
RUN mkdir -p /app/out/rust && \
    cd ./rust && \
    cargo build --release && \
    cp target/release/sorting /app/out/rust/SortingRust

# Stage 4: Build C# binary
FROM mcr.microsoft.com/dotnet/sdk:8.0 AS csharp-builder
WORKDIR /app
COPY dist-sorting/csharp/ ./csharp/
RUN mkdir -p /app/out/csharp && \
    cd ./csharp && \
    dotnet publish -c Release -r linux-x64 --self-contained true -p:PublishSingleFile=true -o /app/publish && \
    cp /app/publish/SortingCSharp /app/out/csharp/SortingCSharp

# Stage 5: Final runner image
FROM node:20-slim
WORKDIR /app

# Install Python3 and Java runtime
RUN apt-get update && \
    apt-get install -y --no-install-recommends python3 openjdk-17-jre-headless && \
    rm -rf /var/lib/apt/lists/*

# Copy package files and install production dependencies
COPY package*.json ./
RUN npm ci --omit=dev

# Copy server code
COPY server.js ./

# Copy pre-compiled Java JAR and Python script
COPY dist-sorting/java/SortingJava.jar ./dist-sorting/java/
COPY dist-sorting/python/sorting.py ./dist-sorting/python/

# Copy compiled binaries from previous stages
COPY --from=cpp-builder /app/out/c/SortingC ./dist-sorting/c/
COPY --from=cpp-builder /app/out/cpp/SortingCpp ./dist-sorting/cpp/
COPY --from=go-builder /app/out/go/SortingGo ./dist-sorting/go/
COPY --from=rust-builder /app/out/rust/SortingRust ./dist-sorting/rust/
COPY --from=csharp-builder /app/out/csharp/SortingCSharp ./dist-sorting/csharp/

# Expose server port
EXPOSE 3000

# Set environment variables
ENV NODE_ENV=production
ENV PORT=3000

# Run the app
CMD ["npm", "start"]
