echo -n Password:
read -s PASSWORD
#PASSWORD="" #password"
echo $PASSWORD | sudo -S apt-get update
echo $PASSWORD | sudo -S apt-get -y install build-essential autoconf libtool pkg-config cmake git clang libgoogle-perftools-dev ocaml ocamlbuild libssl-dev libgmp-dev python libboost-dev libboost-{chrono,log,program-options,date-time,thread,system,filesystem,regex,test}-dev libpq-dev
