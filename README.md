# USS

![USS Enterprise](https://i.imgur.com/G4sYXji.jpg)

A toy lisp based off the book [build your own lisp](http://buildyourownlisp.com/)

Outside of the book i've added:

* [boehm]() garbage collector
* [libedit]() based repl
* [uthash]() lookup tables
* separated function name and args instead of `(fun {yo woo} { ...` it is `(fun 'yo |woo| { ..`
* ' for single qexpr `'` is shorthand for `{}` for example:

```clojure
(== 'x {x}) ; => true
```

* `[]` shorthand for `(list ..)`
* `||` alternative to `{}` for quoted s-expressions used mainly
 to differentiate function params from body for example:

 ```clojure
 (fun 'hello |what| {
   log (++ "Hello! " what)
 })
 ```
* ffi support
  - `ffi-open` to open a c library `(def 'libc (ffi-open "libc.so.6"))`
  - `ffi-def` to define a c function `(def 'libc-puts (ffi-def 'puts {ffi-string} ffi-int32 libc))`
  - `ffi-call` to call a c function `(ffi-call libc-puts "Hello World!")`
* basic tooling
  - repl
  - linter
  - doc-gen
* character primitive
  - `{\H \e \l \l \o \ \W \o \r \l \d}`
* string methods to convert to and from lists


and more hacking check [stdlib](./stdlib/) for examples.
