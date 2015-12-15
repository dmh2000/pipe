#ifndef DATA_H__
#define DATA_H__

#include <cstdio>
#include <cstdint>
/** define a more complex type */
struct lla_t
{
    double m_lat;
    double m_lon;
    double m_alt;

    /** be sure to initialize the data */
    lla_t(): m_lat(0.0),m_lon(0.0),m_alt(0.0)
    {
    }
};


/** create a struct with all data needed through the pipeline */
struct pipe_data_t
{
    uint32_t u32;
    lla_t    lla;
    int      cls;

    /** be sure to initialize the data */
    pipe_data_t(): u32(0),cls(0)
    {
    }

    /** print the data */
    static void print(const char *s,const pipe_data_t &data)
    {
        printf("%s : %u %d %.3f %.3f %.3f\n",s,data.u32,data.cls,data.lla.m_lat,data.lla.m_lon,data.lla.m_alt);
    }
};


#endif