# run with source ./scripts/get-latest-log-file.sh
FILE=$(ls -t log/* | head -n 1)
echo "Latest log file: $FILE"
