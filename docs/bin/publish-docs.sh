#!/usr/bin/env bash

ORIG_DIR="$(pwd)"
cd "$(dirname "$0")"
BIN_DIR="$(pwd)"
BRANCH="$1";

trap "cd '${ORIG_DIR}'" EXIT

# Work in the docs/ directory
cd "${BIN_DIR}/.."

# Check the output directory exists
if [ ! -d "output" ]
then
  echo "ERROR: output directory is missing, generate-docs must be run first" >&2
  exit 1
fi

# Deduce the name of the repository
if [ -z "${GH_REPOS_NAME}" ]
then
  export GH_REPOS_NAME="$(basename $(dirname $PWD))"
fi

# Clone the 'gh-pages' branch
git clone --branch gh-pages --single-branch "https://${GH_TOKEN}@github.com/EOEPCA/${GH_REPOS_NAME}" repos

# Move generated doc outputs to the repos
cd repos
rm -rf ${BRANCH}
mv ../output ${BRANCH}
mv ${BRANCH}/index.pdf ${BRANCH}/EOEPCA-${GH_REPOS_NAME}.pdf

# Prepare the root landing page/README - but don't overwrite if they already exist
if [ ! -e index.html ]; then cp ../gh-page-root.html index.html; fi
if [ ! -e README.adoc ]; then cp ../gh-page-README.adoc README.adoc; fi

# Config git profile for commits
if [ -n "${GH_USER_NAME}" ]; then git config user.name "${GH_USER_NAME}"; fi
if [ -n "${GH_USER_EMAIL}" ]; then git config user.email "${GH_USER_EMAIL}"; fi

# Commit the newly generated docs, and push to upstream
git add . ; git commit -m "Deploy to GitHub Pages @ $(date -u)"
git push
