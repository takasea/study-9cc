#!/bin/bash
assert(){
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s
    cc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

assert 0 "0;"
assert 42 "42;"
assert 21 "5+20-4;"
assert 41 " 12 + 34 - 5;"
assert 47 "5+6*7;"
assert 15 "5*(9-6);"
assert 10 "-10+20;"
assert 15 "30-3*+5;"
assert 10  "20 +2 * -5;"

assert 0 '0==1;'
assert 1 '42==42;'
assert 1 '0!=1;'
assert 0 '42!=42;'

assert 1 '0<1;'
assert 0 '1<1;'
assert 0 '2<1;'
assert 1 '0<=1;'
assert 1 '1<=1;'
assert 0 '2<=1;'

assert 1 '1>0;'
assert 0 '1>1;'
assert 0 '1>2;'
assert 1 '1>=0;'
assert 1 '1>=1;'
assert 0 '1>=2;'

assert 0 'a;'
assert 1 'a = 1;'
assert 30 'z = 15  * 2;'
assert 2 'a = 1; a + 1;'
assert 2 'a = 1 * 2 + 3 - 4; b = a / a; a + b;'
assert 5 'a = 2 == 2; a + a * 4;'
assert 32  'a = 1;b = 2; c = 3;z = 26; a + b + c + z;'
assert 1 'a = 1; b = a; c = b;'

assert 0 'name = 1; age = name - 1;'
assert 7 'one = 1; two = 2; three = 3; one + two * three;'


assert 5 'a = 5; return a;'
assert 10 'retur = 5; return retur + 5;'
assert 10 'returnn = 5; return returnn + 5;'
assert 3 'a = 1; b = 2; return a + b;'
assert 1 'return 1; return 2;'

assert 3 'if(0) return 2; return 3;'
assert 3 'if(1-1) return 2; return 3;'
assert 2 'if(1) return 2; return 3;'
assert 2 'if(2-1) return 2; return 3;'

assert 3 'a=1; if(a==1) a = 2; return 5-a;'
assert 1 'a=0; if(1) a = 1; return 2-a;'


assert 1 'a=3; if(a==3) return 1; else return 5;'
assert 5 'a=3; if(a!=3) return 1; else return 5;'
assert 5 'a=1; if(a!=1) return 1; else return 5;'

assert 1 'a=5; if(a!=1) a = 1; else a = 3;return a;'
assert 3 'a=1; if(a!=1) return 1; else a = 5;  return 3;'
assert 1 'a=1; if(a==2) a=3;'

assert 1 'a=2; return 3-a;'
assert 2 'a=2; if(1) a=1; return 3-a;'


assert 1 'a=2; b=3; 4-b;'
assert 2 'a=2; b=3; c=4; if(0) 1; 5-b;' 
assert 1 'a=2; b=3; c=4; if(0) 1; 5-c;'

assert 2 'a=2; b=3; if(0) 1; 4-a;'
assert 1 'a=2; b=3; if(0) 1; 4-b;'

assert 1 'a=2; if(0) 1; 3-a;' 
assert 1 'a=2; if(0) 1; return 3-a;'

assert 3 'a=1; if(0) return a; else a=3; return a;'
assert 2 'a=2; if(1) return a; else a=3; return a;'


echo OK