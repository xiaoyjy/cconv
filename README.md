# Introduction #

A iconv based simplified-traditional chinese conversion tool

iconv is a open source international text conversion tool, if you do not know iconv, go to http://www.gnu.org/software/libiconv/#introduction to learn more.

As usual for GNU packages:

$ ./configure --prefix=/usr/local<br />
$ make<br />
$ make install<br />

# Details #

Base usage:
Usage: iconv ［OPTION...］ ［FILE］<br />
Convert encoding of given files from one encoding to another.

> Input/Output format specification:
> > -f, --from-code=NAME       encoding of original text
> > -t, --to-code=NAME         encoding for output


> Information:
> > -l, --list                 list all known coded character sets


> Output control:
> > -o, --output=FILE          output file<br /><br />
> > -?, --help                 Give this help list<br />
> > > --usage                Give a short usage message

> > -V, --version              Print program version

Example:<br />
$ echo "美发现号航天飞机, 上头发奖金；头发应该剪了，后天，皇后" | cconv -f utf-8 -t utf8-tw<br />
美發現號航天飛機, 上頭發獎金；頭髮應該剪了，後天，皇后<br />