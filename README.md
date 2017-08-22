To install, do:
```
git clone --recursive https://bitbucket.org/jduggan/smcql-calcite.git
```

To add ObliVM dependencies in Eclipse, add the following files in the build path:
```
./oblivm/lang/lib/oblivm-flexsc-0.2.jar
./oblivm/lang/lib/oblivm-lang.jar
```

Create symbolic links for ObliVM source:
mkdir oblivm
git submodule add https://jduggan@bitbucket.org/johesbater/oblivmgcsmc.git oblivm/gc
git submodule add https://jduggan@bitbucket.org/johesbater/oblivmlangsmc.git oblivm/lang 

 ln -s /Users/jennie/projects/smcql/nu-repos/smcql-calcite/oblivm/lang/java/com/oblivm/compiler  /Users/jennie/projects/smcql/nu-repos/smcql-calcite/src/main/java/com/oblivm/compiler

 ln -s /Users/jennie/projects/smcql/nu-repos/smcql-calcite/oblivm/gc/java/com/oblivm/backend  /Users/jennie/projects/smcql/nu-repos/smcql-calcite/src/main/java/com/oblivm/backend