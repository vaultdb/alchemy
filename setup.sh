#!/bin/bash

# install dependencies
sudo apt-get update
sudo apt-get install -y build-essential
sudo apt-get install -y cmake
sudo apt-get install -y git
sudo apt-get install -y libssl-dev
sudo apt-get install -y sudo
sudo apt-get install -y wget
sudo apt-get install -y maven
sudo apt-get install -y default-jdk
sudo apt-get install -y libgflags-dev
sudo apt-get install -y libgmp-dev
sudo apt-get install -y libpq-dev
sudo apt-get install -y libboost-all-dev
sudo apt-get install -y postgresql
sudo apt-get install -y postgresql-contrib
sudo apt-get install -y postgresql-client
sudo apt-get install -y python
sudo apt-get install -y openssh-server
sudo apt-get install -y emacs
sudo apt-get install -y libprotobuf-dev
sudo apt-get install -y protobuf-c-compiler
sudo apt-get install -y libqt5webkit5
sudo apt-get install -y tmux
sudo apt-get install -y valgrind
sudo apt-get install -y dos2unix


#setup test databases for unit tests

# Start PostgreSQL if you're installing it for the first time
sudo /etc/init.d/postgresql start

# Create a new user (if not created yet)
sudo -i -u postgres createuser -l -s root

# Create the vaultdb role and database
sudo psql postgres -c "CREATE ROLE vaultdb WITH LOGIN SUPERUSER;" || echo "Role may already exist."
createdb vaultdb || echo "Database may already exist."

# Load specific SQL files into specific databases
dropdb -U vaultdb --if-exists tpch_unioned
createdb -U vaultdb tpch_unioned || { echo "Creating tpch_unioned failed"; exit 1; }
psql -U vaultdb tpch_unioned < dbs/tpch_unioned.sql || { echo "Loading tpch_unioned failed"; exit 1; }
echo "tpch_unioned created!"

dropdb -U vaultdb --if-exists tpch_empty
createdb -U vaultdb tpch_empty || { echo "Creating tpch_empty failed"; exit 1; }
psql -U vaultdb tpch_empty < dbs/tpch_empty.sql || { echo "Loading tpch_empty failed"; exit 1; }
echo "tpch_empty created!"

# Execute psql commands non-interactively
psql -U vaultdb <<EOF
\i truncate-tpch-set.sql
EOF

# Run the Bash script
bash create-order-keys.sh

echo "Database setup complete!"

# needed for macosx, may need to check for os name on this first:
ln -s /usr/local/opt/openssl/include/openssl /usr/local/include
ln -s /usr/local/opt/openssl/lib/libssl.1.0.0.dylib /usr/local/lib/
ln -s /usr/local/opt/openssl/lib/libcrypto.dylib /usr/local/lib/
ln -s /usr/local/opt/openssl/lib/libssl.dylib /usr/local/lib/

#download emp to dep
cd deps/emp
bash install.sh 

#install correct pqxx version
cd ..
git clone --branch 7.7.4 https://github.com/jtv/libpqxx.git
cd libpqxx
 ./configure  --disable-documentation --enable-shared CXXFLAGS="-std=c++17 -O2"
make 
sudo make install

cd ..
git clone --branch v1.13.0 https://github.com/google/googletest.git
cd googletest
cmake .
make all
sudo make install


#end
echo "Setup completed successfully."
exit 0
