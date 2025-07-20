#ifdef _WIN32
#define DLLFUNC __declspec(dllexport)
#else
#define DLLFUNC
#endif

DLLFUNC int add(int a, int b ) {
    return a + b;
}
