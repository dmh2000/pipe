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
        /**
         * a pointer to the next 'pipe' in the pipeline that this element pipes its output to
         */
        std::shared_ptr<pipe_t> m_next;

        /**
         * a Callable that executes the functor for this element in the pipeline
         */
        std::function<bool(T&)> m_exec;

        /**
        * default constructor
        * no next until it is set by 'pipe'. if not set, the pipeline stops here
        */
        pipe_t(): m_next(nullptr),m_exec(nullptr) {}


        /**
         * initializing constructor
         * @param next a pointer to the next 'pipe'
         * @param exec a callable used by this pipe
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
        * @param p next pipe in the pipeline
        */
        virtual std::shared_ptr<pipe_t> pipe(std::shared_ptr<pipe_t> next)
        {
            // save the next one and return it
            m_next = next;
            return next;
        }

        /**
        * call next object in chain if there is one
        */
        virtual void next(const T &t)
        {
            if(m_next != nullptr) {
                m_next->exec(t);
            }
        }

        /**
        * subclasses implement 'exec' and call 'next' i
        * this is used if the input pipe is via a functor or lambda with operator()
        * rather than its own exec. these functions must return false to continue execution
        * or true to terminate
        *
        * Uses of this class (pipe) can either implement 'exec' or pass in a callable to be used in the default exec
        * To implement an own exec function, inherit this class. the exec function must call 'next' with the
        * output data from this state
        *
        * to use the default 'exec' function, create the pipe with a callable (functor, std::function, lambda) 
        * and the lambda returns false if it has more operations or true if it is done
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
        void exec(const int &data) {
        // process the point
        // possibly creating a new one
        // point = ...
        next(i);
        // also its ok to call next any number of times as new data is generated
        for(i=0;i<N;++i) {
            newdata = ....use input data to create a new output object
            next(newdata)
        }
        */

    };



    /**
     * create a pipe from a function, functor or lambda that is Callable
     * callables include std::function, lambda, functor with operator(), or a plain old function
     * this function may copy construct the input functor multiple times. be sure to have a valid copy constructor
     * if the default isn't enough
     */
    template<typename T> std::shared_ptr<pipe_t<T> > pipe(std::function<bool(T &)> g)
    {
        return std::make_shared<pipe_t<T> >(nullptr,g);
    }


    /**
     * compose some number of piped functions
     */
    template<typename T,typename... Args> std::shared_ptr<pipe_t<T> > compose(const Args& ...args)
    {
        size_t len = sizeof...(args);                     // get number of functions
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

