# syntax=docker/dockerfile:1.4

FROM --platform=$BUILDPLATFORM node:17.0.1-bullseye-slim as builder

RUN mkdir /project
WORKDIR /project

RUN npm install -g @angular/cli@13

COPY package.json package-lock.json ./
RUN npm ci

COPY . .
CMD ["ng", "serve", "--host", "0.0.0.0"]

FROM builder as dev-envs

RUN <<EOF
apt-get update
apt-get install -y --no-install-recommends git
EOF

RUN <<EOF
useradd -s /bin/bash -m vscode
groupadd docker
usermod -aG docker vscode
EOF
#  install Docker tools (cli, buildx, compose)
COPY --from=gloursdocker/docker / /

ENTRYPOINT ["ng", "serve", "--host", "0.0.0.0", "--disable-host-check"]

# FROM node:16-alpine AS builder
# ARG CONFIGURATION='development'

# # Make /app as working directory
# WORKDIR /app

# # Copy package.json file
# COPY package.json .

# # Install dependencies
# RUN npm install --legacy-peer-deps

# # Copy the source code to the /app directory
# COPY . .

# # Build the application
# RUN npm run build --  --output-path=dist --configuration=$CONFIGURATION --output-hashing=all
