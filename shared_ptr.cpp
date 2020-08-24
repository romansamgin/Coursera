struct Expression;
struct Number;
struct BinaryOperation;

struct SharedPtr
{
    private:      
        Expression *p_;
        size_t  *k_;
    public:
    
    void swap(SharedPtr& a)
    {
        Expression* const p = p_;
        p_ = a.p_;
        a.p_ = p;

        size_t* const k = k_;
        k_ = a.k_;
        a.k_ = k;
    }
    
    Expression* get() const { return this->p_; }
    
    explicit SharedPtr(Expression *ptr = 0) : p_(ptr), k_(0)
    {
            k_ = new size_t(1);
    }
    
     ~SharedPtr() 
     { 
         --*k_;
         if (*k_ == 0)
         {
             delete p_;
             delete k_;
         }
     }
      
    
    SharedPtr(const SharedPtr &ptr) : p_(ptr.p_), k_(ptr.k_)
    {
        ++*k_;
    }
    
    

    
    SharedPtr& operator=(const SharedPtr& ptr)
    {
        if (this != &ptr)
        {
            {
                delete k_;
                k_ = ptr.k_;
                ++* k_;
                delete p_;
                p_ = ptr.p_;
            }        
        }
        return *this;
    }
    
    
    void reset(Expression* ptr = 0)
    {
        if (-- * k_ == 0)
        {
            delete p_;
            delete k_;
            k_ = new size_t(1);

        }
        p_ = ptr;
    }
      
    
    Expression& operator*() const
    {
        return *p_;
    }
    Expression* operator->() const
    {
        return p_;
    }
};