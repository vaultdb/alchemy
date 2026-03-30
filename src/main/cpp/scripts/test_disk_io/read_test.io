[readtest]
filename=/home/vaultdb/src/main/cpp/shares/tpch_unioned_150/lineitem.1   # Replace this with the path to the file
rw=read                       # Specifies read operation
size=100%                     # Read the entire file (adjust if you want to limit it)
numjobs=1                     # Number of jobs (parallel workers)
loops=1                       # Number of times to read the file (loops)
ioengine=sync                 # Use synchronous I/O
direct=1                      # Bypass cache for more accurate disk speed measurement
