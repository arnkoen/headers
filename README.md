This repo contains small and simple single header utilities.
Some of the headers have an implementation part, which is disabled by default.
To use them, drop them into your project, #define UTILNAME_IMPL in one c file and include them.
For licensing information and credits, please have a look at the top of the respective header.
All of this mini libs should work on Linux(gcc) and Windows(cl), I don't own a mac and rarely use other platforms.
You can use your own malloc() and free() replacements for most of the headers. If you do so, please make sure,
to replace *both* functions with your own stuff.
