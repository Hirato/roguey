#include <cstdio>
#include <cstring>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <cstdarg>
#include <ctime>

//mersenne's twister sourced from cube2
extern unsigned int randomMT();
#define rnd(x) ((int)(randomMT()&0xFFFFFF)%(x))
#define rndscale(x) (float((randomMT()&0xFFFFFF)*double(x)/double(0xFFFFFF)))
#define detrnd(s, x) ((int)(((((uint)(s))*1103515245+12345)>>16)%(x)))

#define loop(v, n) for(int v = 0; v < int(n); v++)
#define loopv(v, vec) for(int v = 0; v < (vec).length(); v++)
#define looprev(v, n) for(int v = (n) - 1; v >= 0; v--)
#define loopvrev(v, vec) for(int v = (vec).length() - 1; v >= 0; v--)

#define loopi(n) loop(i, n)
#define loopj(n) loop(j, n)
#define loopk(n) loop(k, n)

#define loopvi(n) loopv(i, n)
#define loopvj(n) loopv(j, n)
#define loopvk(n) loopv(k, n)

inline void *operator new(size_t size)
{
    void *p = malloc(size);
    if(!p) abort();
    return p;
}
inline void *operator new[](size_t size)
{
    void *p = malloc(size);
    if(!p) abort();
    return p;
}
inline void operator delete(void *p) { free(p); }
inline void operator delete[](void *p) { free(p); }

inline void *operator new(size_t, void *p) { return p; }
inline void *operator new[](size_t, void *p) { return p; }
inline void operator delete(void *, void *) {}
inline void operator delete[](void *, void *) {}

template<class T>
T min(T a, T b)
{
	return a < b ? a : b;
}

template<class T>
T max(T a, T b)
{
	return a > b ? a : b;
}

template<class T>
T clamp(T l, T u, T v) //min, max, val
{
	return max(l, min(u, v));
}

template<class T>
struct vector
{
	T *buf;
	int ulen, alen;

	vector() : buf(NULL), ulen(0), alen(0) {}
	~vector() {delete[] buf;}

	T pop()
	{
		return buf[--ulen];
	}

	void clear()
	{
		ulen = 0;
	}

	void deletecontents()
	{
		while(ulen) delete pop();
	}

	void deletearrays()
	{
		while(ulen) delete[] pop();
	}

	void growbuf(int size)
	{
		if(alen >= size) return;
		int newlen = max(1, alen);
		while(newlen < size) newlen *= 2;

		T *newbuf = new T[newlen];
		if(buf)
		{
			memcpy(newbuf, buf, sizeof(T) * ulen);
			delete[] buf;
		}

		buf = newbuf;
		alen = newlen;
	}

	void removeobject(void *ptr)
	{
		loopi(ulen) if(buf[i]==ptr) remove(i--);
	}

	void reserve(int size)
	{
		growbuf(ulen + size);
	}

	T &add(const T &x)
	{
		growbuf(ulen + 1);
		return (buf[ulen++] = x);
	}

	T remove(int i)
	{
		T e = buf[i];
		for(int p = i + 1; p < ulen; p++)
			buf[p-1] = buf[p];
		ulen--;
		return e;
	}

	T quickremove(int i)
	{
		T e = buf[i];
		buf[i] = buf[--ulen];
		return e;
	}

	void remove(int i, int n)
	{
		for(int p = i + n; p < ulen; p++)
			buf[p-n] = buf[p];
		ulen -= n;
	}

	void quickremove(int i, int n)
	{
		ulen -= n;
		while(n > 0 && i < ulen)
		{
			buf[i] = buf[ulen + n];
			n--; i++;
		}
		ulen += n;
	}

	inline T &operator[](int n) { return buf[n]; }
	const inline T &operator[](int n) const { return buf[n]; }
	int length() const {return ulen;}
	int capacity() const {return alen;}
	bool inrange(int n) {return n >= 0 && n < ulen; }
};

struct vec2
{
	int x, y;

	vec2(int x, int y) : x(x), y(y) {}
	~vec2() {}

	//float magnitude()
	//float dist(vec2 &other)
	//vec2 &mul()
	//vec2 &sub()
	//vec2 &add()
	//vec2 &div()
	//vec2 &normalise()
};

/// cube 2 hashtable implementation and strings
/// licensed as zlib

#define _vsnprintf vsnprintf
// easy safe strings

#define MAXSTRLEN 260
typedef char string[MAXSTRLEN];

inline void vformatstring(char *d, const char *fmt, va_list v, int len = MAXSTRLEN) { _vsnprintf(d, len, fmt, v); d[len-1] = 0; }
inline char *copystring(char *d, const char *s, size_t len = MAXSTRLEN) { strncpy(d, s, len); d[len-1] = 0; return d; }
inline char *concatstring(char *d, const char *s, size_t len = MAXSTRLEN) { size_t used = strlen(d); return used < len ? copystring(d+used, s, len-used) : d; }

struct stringformatter
{
    char *buf;
    stringformatter(char *buf): buf((char *)buf) {}
    void operator()(const char *fmt, ...)
    {
        va_list v;
        va_start(v, fmt);
        vformatstring(buf, fmt, v);
        va_end(v);
    }
};

#define formatstring(d) stringformatter((char *)d)
#define defformatstring(d) string d; formatstring(d)
#define defvformatstring(d,last,fmt) string d; { va_list ap; va_start(ap, last); vformatstring(d, fmt, ap); va_end(ap); }

inline char *newstring(size_t l)                { return new char[l+1]; }
inline char *newstring(const char *s, size_t l) { return copystring(newstring(l), s, l+1); }
inline char *newstring(const char *s)           { return newstring(s, strlen(s));          }
inline char *newstringbuf(const char *s)        { return newstring(s, MAXSTRLEN-1);       }

static inline unsigned int hthash(const char *key)
{
	unsigned int h = 5381;
	while(*key) h = ((h << 5) + h) ^ *(key++); // bernstein k=33 xor
	return h;
}

static inline bool htcmp(const char *x, const char *y)
{
    return !strcmp(x, y);
}

static inline unsigned int hthash(int key)
{
    return key;
}

static inline bool htcmp(int x, int y)
{
    return x==y;
}

template<class T> struct hashset
{
    typedef T elem;
    typedef const T const_elem;

    enum { CHUNKSIZE = 64 };

    struct chain { T elem; chain *next; };
    struct chainchunk { chain chains[CHUNKSIZE]; chainchunk *next; };

    int size;
    int numelems;
    chain **chains;

    chainchunk *chunks;
    chain *unused;

    hashset(int size = 1<<10)
      : size(size)
    {
        numelems = 0;
        chunks = NULL;
        unused = NULL;
        chains = new chain *[size];
        loopi(size) chains[i] = NULL;
    }

    ~hashset()
    {
        delete[] chains;
        deletechunks();
    }

    chain *insert(unsigned int h)
    {
        if(!unused)
        {
            chainchunk *chunk = new chainchunk;
            chunk->next = chunks;
            chunks = chunk;
            loopi(CHUNKSIZE-1) chunk->chains[i].next = &chunk->chains[i+1];
            chunk->chains[CHUNKSIZE-1].next = unused;
            unused = chunk->chains;
        }
        chain *c = unused;
        unused = unused->next;
        c->next = chains[h];
        chains[h] = c;
        numelems++;
        return c;
    }

    #define HTFIND(key, success, fail) \
        unsigned int h = hthash(key)&(this->size-1); \
        for(chain *c = this->chains[h]; c; c = c->next) \
        { \
            if(htcmp(key, c->elem)) return (success); \
        } \
        return (fail);

    template<class K>
    T *access(const K &key)
    {
        HTFIND(key, &c->elem, NULL);
    }

    template<class K>
    T &access(const K &key, const T &elem)
    {
        HTFIND(key, c->elem, insert(h)->elem = elem);
    }

    template<class K>
    T &operator[](const K &key)
    {
        HTFIND(key, c->elem, insert(h)->elem);
    }

    template<class K>
    bool remove(const K &key)
    {
        unsigned int h = hthash(key)&(size-1);
        for(chain **p = &chains[h], *c = chains[h]; c; p = &c->next, c = c->next)
        {
            if(htcmp(key, c->elem))
            {
                *p = c->next;
                c->elem.~T();
                new (&c->elem) T;
                c->next = unused;
                unused = c;
                numelems--;
                return true;
            }
        }
        return false;
    }

    void deletechunks()
    {
        for(chainchunk *nextchunk; chunks; chunks = nextchunk)
        {
            nextchunk = chunks->next;
            delete chunks;
        }
    }

    void clear()
    {
        if(!numelems) return;
        loopi(size) chains[i] = NULL;
        numelems = 0;
        unused = NULL;
        deletechunks();
    }

    static inline chain *getnext(void *i) { return ((chain *)i)->next; }
    static inline T &getdata(void *i) { return ((chain *)i)->elem; }
};

template<class K, class T> struct hashtableentry
{
    K key;
    T data;

    hashtableentry() {}
    hashtableentry(const K &key, const T &data) : key(key), data(data) {}
};

template<class U, class K, class T>
static inline bool htcmp(const U *x, const hashtableentry<K, T> &y)
{
    return htcmp(x, y.key);
}

template<class U, class K, class T>
static inline bool htcmp(const U &x, const hashtableentry<K, T> &y)
{
    return htcmp(x, y.key);
}

template<class K, class T> struct hashtable : hashset<hashtableentry<K, T> >
{
    typedef hashtableentry<K, T> entry;
    typedef struct hashset<entry>::chain chain;
    typedef K key;
    typedef T value;

    hashtable(int size = 1<<10) : hashset<entry>(size) {}

    entry &insert(const K &key, unsigned int h)
    {
        chain *c = hashset<entry>::insert(h);
        c->elem.key = key;
        return c->elem;
    }

    T *access(const K &key)
    {
        HTFIND(key, &c->elem.data, NULL);
    }

    T &access(const K &key, const T &data)
    {
        HTFIND(key, c->elem.data, insert(key, h).data = data);
    }

    T &operator[](const K &key)
    {
        HTFIND(key, c->elem.data, insert(key, h).data);
    }

    static inline chain *getnext(void *i) { return ((chain *)i)->next; }
    static inline K &getkey(void *i) { return ((chain *)i)->elem.key; }
    static inline T &getdata(void *i) { return ((chain *)i)->elem.data; }
};

#define enumerates(ht,t,e,b)      loopi((ht).size)  for(hashset<t>::chain *enumc = (ht).chains[i]; enumc;) { t &e = enumc->elem; enumc = enumc->next; b; }
#define enumeratekt(ht,k,e,t,f,b) loopi((ht).size)  for(hashtable<k,t>::chain *enumc = (ht).chains[i]; enumc;) { const hashtable<k,t>::key &e = enumc->elem.key; t &f = enumc->elem.data; enumc = enumc->next; b; }
#define enumerate(ht,t,e,b)       loopi((ht).size) for(void *enumc = (ht).chains[i]; enumc;) { t &e = (ht).getdata(enumc); enumc = (ht).getnext(enumc); b; }

///END hashtable stuffies


struct stream
{

	stream() {}
	virtual ~stream() {}
};

struct filestream : stream
{
	FILE *file;

	filestream() {}
	~filestream() {}
};