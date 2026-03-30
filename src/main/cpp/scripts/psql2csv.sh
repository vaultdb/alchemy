sort t | sed 's/^/\(/g' | sed 's/$/\)/g'  | sed 's/\|/, /g' | sed 's/\ //g' > tmp
