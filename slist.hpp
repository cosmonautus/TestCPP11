#ifndef SLIST_HPP
#define SLIST_HPP

struct slink 
{
    slink* next;
    slink() 
    { 
        next = nullptr; 
    }
    slink(slink* p) 
    { 
        next = p; 
    }
};

template<class T>
struct Tlink : public slink 
{
    T info;
    Tlink(const T& a) : info(a) {}
};

class slist_base 
{
private:
    slink* last = nullptr;

public:
    slink* insert(slink* p)
    {
        if (last != nullptr)
        {
            p->next = last->next;
            last->next = p;
        }
        else
        {
            last = p;
            last->next = p;
        }
        return p;
    }
    slink* append(slink* p)
    {
        if (last != nullptr)
        {
            p->next = last->next;
            last->next = p;
            last = p;
        }
        else
        {
            last = p;
            last->next = p;
        }
        return p;
    }
    slink* get()
    {
        if (last != nullptr)
        {
            slink* res = last->next;
            if (res != last)
            {
                last->next = res->next;
            }
            else
                last = nullptr;
            res->next = nullptr;
            return res;
        }
        else
            return nullptr;
    }
};

template<class T>
class Slist : private slist_base
{
public:
    void insert(const T& a) { slist_base::insert(new Tlink<T>(a)); }
    void append(const T& a) { slist_base::append(new Tlink<T>(a)); }
    T get()
    {
        Tlink<T>* lnk = static_cast<Tlink<T>*>(slist_base::get());
        T i = lnk->info;
        delete lnk;
        return i;
    }
};

#endif //SLIST_HPP