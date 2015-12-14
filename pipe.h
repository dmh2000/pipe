#ifndef PIPE_H__
#define PIPE_H__

#include <memory>
#include <functional>

namespace Pipe {

    /**
    * support for a class that needs to
    * receive input, process it and pass it to the next
    * element in a pipeline chain
    */
    template<typename T> struct pipe_t
    {
        std::shared_ptr<pipe_t> m_next;
        std::function<bool(T&)> m_exec;

        /**
        * default constructor
        * no next until it is set by 'pipe'
        */
        pipe_t(): m_next(nullptr),m_exec(nullptr) {}


        /**
         * initializing constructor
         */
        pipe_t(std::shared_ptr<pipe_t> next,std::function<bool(T&)> exec): m_next(next),m_exec(exec) 
        {
        }

        /**
         * set the next function
         */
        void setNext(std::shared_ptr<pipe_t> next) {
            m_next = next;
        }


        /**
        * virtual destructor since this is inherited
        */
        virtual ~pipe_t() {
            printf("~pipe_t\n");
        }

        /**
        * connect 'this' pipe to another pipe as its 'next' in the pipeline
        * set the destintation object for the subsequent exec-next call
        * return the destination so the calls can be chained
        */
        virtual std::shared_ptr<pipe_t> pipe(std::shared_ptr<pipe_t> p)
        {
            m_next = p;
            return p;
        }

        /**
        * call next object in chain
        */
        virtual void next(const T &t)
        {
            if(m_next != nullptr) {
                m_next->exec(t);
            }
        }

        /**
        * subclasses implement 'exec' and call 'next' if there is one
        * this implementation is a passthrough pipe
        */
        virtual void  exec(const T &t)
        {
            T x = t;
            bool done;

            done = m_exec(x);
            while(!done) {
                next(x);
                done = m_exec(x);
            }
            next(x);
        }

        /*
        in subclass:
        void exec(const LLA &pos) {
        // process the point
        // possibly creating a new one
        // point = ...
        next(point);
        // also its ok to call next any number of times as new data is generated
        for(i=0;i<N;++i) {
            // point = ....
            next(point)
        }
        */

    };


    /**
     * create a pipe with a specified callable function or functor
     * the function can be a functor, a plain old function or a lambda 
     * depending on whether you need the function to have state
     *
     * 1. all the pipes functions must have the same argument type bool f(T &t);
     * 2. the piped functions all get the same input type, a single reference
     *    to a mutable value of the type specified in the template specialization
     * 3. if the value is to be modified, change it directory via the reference
     * 4. the functions may generate more than one input to the pipe. they should return 
     *    true (done) if no more input, false if more input is needed
     *
     * =========================================
     * example for a one to one transform
     * =========================================
     * struct functor
     * {
     *     bool operator()(T &t)
     *     {
     *         printf("3");
     *         return true; // 'done'
     *     }
     * };
     *  
     * =========================================
     * example for a one to many transform
     * =========================================
     * struct functor {
     *     int count;
     *     functor() : count(10) {}
     * 
     *     bool operator()(T &t) {
     *         print("2");
     *         count--;
     *         // return false until all output is produced
     *         return (count == 0);
     *     }
     * };
     *
     */
    template<typename T> std::shared_ptr<pipe_t<T> > pipe(std::function<bool(T &)> f)
    {
        return std::make_shared<pipe_t<T> >(nullptr,f);
    }

    /**
     * compose some number of piped functions
     */
    template<typename T,typename... Args> std::shared_ptr<pipe_t<T> > compose(const Args& ...args)
    {
        size_t len = sizeof...(args);                   // get number of functions
        std::shared_ptr<pipe_t<T> >  args[] = {args...};  // deconstruct the argument pack

        if (len == 0) {
            // no pipes
            return nullptr;
        }

        if (len == 1) {
            // no next
            return args[0];
        }
    
        // compose from last to first
        // example for 4 arguments:
        //     connect pipe from args[2] to args[3]
        //     connect pipe from args[1] to args[2]
        //     connect pipe from args[0] to args[1]
        //     return args[0]
        for(size_t i=len-1;i>0;i--) {
            args[i-1]->setNext(args[i]);
        }

        // return first
        return args[0];
    }
}
#endif

