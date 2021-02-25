#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../http.h"

// This file created to compare different types for finding words matching using if-else statement and switch in case sensitive comparison.
// There is time calculation spent by different approaches. In the end consumed time by functions will be shown.
// These approaches has different perfomance if compile with no optimization(or default -O1 in gcc). But if specify -Ofast all functions works almost equally.
// To compile: gcc compare_case_sensitive.c -o compare_case_sensitive -Ofast

http_method_t find_http_method_if_else(const char *sval)
{
    if(sval[0] == 'G' && sval[1] == 'E' && sval[2] == 'T')
        return GET;
    else if(sval[0] == 'P' && sval[1] == 'O' && sval[2] == 'S' && sval[3] == 'T')
        return POST;
    else if (sval[0] == 'H' && sval[1] == 'E' && sval[2] == 'A' && sval[3] == 'D')
        return HEAD;
    else if(sval[0] == 'P' && sval[1] == 'U' && sval[2] == 'T')
        return PUT;
    else if(sval[0] == 'D' && sval[1] == 'E' && sval[2] == 'L' && sval[3] == 'E' && sval[4] == 'T' && sval[5] == 'E')
        return DELETE;
    else if(sval[0] == 'C' && sval[1] == 'O' && sval[2] == 'N' && sval[3] == 'N' && sval[4] == 'E' && sval[5] == 'C' && sval[6] == 'T')
        return CONNECT;
    else if(sval[0] == 'O' && sval[1] == 'P' && sval[2] == 'T' && sval[3] == 'I' && sval[4] == 'O' && sval[5] == 'N' && sval[6] == 'S')
        return OPTIONS;
    else if(sval[0] == 'T' && sval[1] == 'R' && sval[2] == 'A' && sval[3] == 'C' && sval[4] == 'E')
        return TRACE;
    else if(sval[0] == 'P' && sval[1] == 'A' && sval[2] == 'T' && sval[3] == 'C' && sval[4] == 'H')
        return PATCH;

    return INVALID_METHOD;

}

http_method_t find_http_method_switch(const char *sval)
{
    switch (sval[0])
    {
    case 'G':
        // GET
        switch(sval[1])
        {
        case 'E':
            switch (sval[2])
            {
            case 'T':
                return GET;
            }
        }
        break;
    case 'P':
        // POST, PUT, PATCH
        switch(sval[1])
        {
        case 'O':
            // POST
            switch(sval[2])
            {
            case 'S':
                switch(sval[3])
                {
                case 'T':
                    return POST;
                }
            }
            break;
        case 'U':
            //PUT
            switch (sval[2])
            {
            case 'T':
                return PUT;
            }
            break;
        case 'A':
            //PATCH
            switch(sval[2])
            {
            case 'T':
                switch(sval[3])
                {
                case 'C':
                    switch(sval[4])
                    {
                    case 'H':
                        return PATCH;
                    }
                }
            }
            break;
        }
        break;
    case 'H':
        // HEAD
        switch(sval[1])
        {
        case 'E':
            switch(sval[2])
            {
            case 'A':
                switch(sval[3])
                {
                case 'D':
                    return HEAD;
                }
            }
        }
        break;
    case 'D':
        // DELETE
        switch (sval[1])
        {
        case 'E':
            switch(sval[2])
            {
            case 'L':
                switch(sval[3])
                {
                case 'E':
                    switch(sval[4])
                    {
                    case 'T':
                        switch(sval[5])
                        {
                        case 'E':
                            return DELETE;
                        }
                    }
                }
            }
        }
        break;
    case 'C':
        // CONNECT
        switch (sval[1])
        {
        case 'O':
            switch (sval[2])
            {
            case 'N':
                switch (sval[3])
                {
                case 'N':
                    switch (sval[4])
                    {
                    case 'E':
                        switch (sval[5])
                        {
                        case 'C':
                            switch (sval[6])
                            {
                            case 'T':
                                return CONNECT;
                            }
                        }
                    }
                }
            }
        }
        break;
    case 'O':
        // OPTIONS
        switch (sval[1])
        {
        case 'P':
            switch (sval[2])
            {
            case 'T':
                switch (sval[3])
                {
                case 'I':
                    switch (sval[4])
                    {
                    case 'O':
                        switch (sval[5])
                        {
                        case 'N':
                            switch (sval[6])
                            {
                            case 'S':
                                return OPTIONS;
                            }
                        }
                    }
                }
            }
        }
        break;
    case 'T':
        // TRACE
        switch(sval[1])
        {
        case 'R':
            switch (sval[2])
            {
            case 'A':
                switch (sval[3])
                {
                case 'C':
                    switch (sval[4])
                    {
                    case 'E':
                        return TRACE;
                    }
                }
            }
        }
        break;
    }
    return INVALID_METHOD;
}
//switch1 is the faster
http_method_t find_http_method_switch1(const char *sval)
{
//   http_method_t result=INVALID_METHOD;
   switch (sval[0])
   {
   case 'G':
       // GET
       switch(sval[1])
       {
       case 'E':
           switch (sval[2])
           {
           case 'T':
               //result = GET;
               return GET;
               //break;
           default:
               return INVALID_METHOD;
           }
           break;
       default:
           return INVALID_METHOD;
       } 
       break;
   case 'P':
       // POST, PUT, PATCH
       switch(sval[1])
       {
       case 'O':
           // POST
           switch(sval[2])
           {
           case 'S':
               switch(sval[3])
               {
               case 'T':
                   //result = POST;
                   return POST;
                //   break;
               default:
                   return INVALID_METHOD;
               }
               break;
           default:
               return INVALID_METHOD;
           }
           break;

       case 'U':
           //PUT
           switch (sval[2])
           {
           case 'T':
               //result = PUT;
               return PUT;
               //break;
           default:
               return INVALID_METHOD;
           }
           break;
       case 'A':
           //PATCH
           switch(sval[2])
           {
           case 'T':
               switch(sval[3])
               {
               case 'C':
                   switch(sval[4])
                   {
                   case 'H':
                       return PATCH;
                    //   break;
                   default:
                       return INVALID_METHOD;
                   }
                   break;
               default:
                   return INVALID_METHOD;
               }
               break;
           default:
               return INVALID_METHOD;
           }
           break;
       default:
           return INVALID_METHOD;
       }
       break;
   case 'H':
       // HEAD
       switch(sval[1])
       {
       case 'E':
           switch(sval[2])
           {
           case 'A':
               switch(sval[3])
               {
               case 'D':
                   //result = HEAD;
                   return HEAD;
                   //break;
               default:
                   return INVALID_METHOD;
               }
               break;
           default:
               return INVALID_METHOD;
           }
           break;
       default:
           return INVALID_METHOD;
       }
       break;
   case 'D':
       // DELETE
       switch (sval[1])
       {
       case 'E':
           switch(sval[2])
           {
           case 'L':
               switch(sval[3])
               {
               case 'E':
                   switch(sval[4])
                   {
                   case 'T':
                       switch(sval[5])
                       {
                       case 'E':
                           //result = DELETE;
                           return DELETE;
                     //      break;
                       }
                       break;
                   default:
                       return INVALID_METHOD;
                   }
                   break;
               default:
                   return INVALID_METHOD;
               }
               break;
           default:
               return INVALID_METHOD;
           }
           break;
       default:
           return INVALID_METHOD;
       }
       break;
   case 'C':
       // CONNECT
       switch (sval[1])
       {
       case 'O':
           switch (sval[2])
           {
           case 'N':
               switch (sval[3])
               {
               case 'N':
                   switch (sval[4])
                   {
                   case 'E':
                       switch (sval[5])
                       {
                       case 'C':
                           switch (sval[6])
                           {
                           case 'T':
                               //result = CONNECT;
                               return CONNECT;
                    //           break;
                           }
                           break;
                       default:
                           return INVALID_METHOD;
                       }
                       break;
                   default:
                       return INVALID_METHOD;
                   }
                   break;
               default:
                   return INVALID_METHOD;
               }
               break;
           default:
               return INVALID_METHOD;
           }
           break;
       default:
           return INVALID_METHOD;
       }
       break;
   case 'O':
       // OPTIONS
       switch (sval[1])
       {
       case 'P':
           switch (sval[2])
           {
           case 'T':
               switch (sval[3])
               {
               case 'I':
                   switch (sval[4])
                   {
                   case 'O':
                       switch (sval[5])
                       {
                       case 'N':
                           switch (sval[6])
                           {
                           case 'S':
                               //result = OPTIONS;
                               return OPTIONS;
                   //            break;
                           default:
                               return INVALID_METHOD;
                           }
                           break;
                       default:
                           return INVALID_METHOD;
                       }
                       break;
                   default:
                       return INVALID_METHOD;
                   }
                   break;
               default:
                   return INVALID_METHOD;
               }
               break;
           default:
               return INVALID_METHOD;
           }
           break;
       default:
           return INVALID_METHOD;
       }
       break;
   case 'T':
       // TRACE
       switch(sval[1])
       {
       case 'R':
           switch (sval[2])
           {
           case 'A':
               switch (sval[3])
               {
               case 'C':
                   switch (sval[4])
                   {
                   case 'E':
                       //result = TRACE;
                       return TRACE;
                   default:
                       return INVALID_METHOD;
                   }
                   break;
               default:
                   return INVALID_METHOD;
               }
               break;
           default:
               return INVALID_METHOD;
           }
           break;
       default:
           return INVALID_METHOD;
       }
       break;
   default:
       return INVALID_METHOD;
   }

//   return result;
}

int main(int num, char ** args)
{
clock_t start, end;
double cpu_time_used;

char method1[]="GET";
char method2[]="HEAD";
char method3[]="OPTIONS";
char wrong_method1[]="GET";// wrong GET
char wrong_method2[]="OPTIONS";//wrong OPTIONS
http_method_t r1,r2,r3,r4,r5;
printf("method1: %s, method2: %s, method3: %s, method4: %s, method5: %s\n", method1,method2,method3,wrong_method1,wrong_method2);

start = clock();
for(int i=0; i < 10000; i++)
{
    r1=find_http_method_switch(method1);
    r2=find_http_method_switch(method2);
    r3=find_http_method_switch(method3);
    r4=find_http_method_switch(wrong_method1);
    r5=find_http_method_switch(wrong_method2);

}
end = clock();
cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
printf("\nswitch parsing functions took: %f\n", cpu_time_used);
printf("results\n r1=%i,r2=%i,r3=%i,r4=%i,r5=%i\n",r1,r2,r3,r4,r5);



start = clock();
for(int i=0; i < 10000; i++)
{
    r1=find_http_method_if_else(method1);
    r2=find_http_method_if_else(method2);
    r3=find_http_method_if_else(method3);
    r4=find_http_method_if_else(wrong_method1);
    r5=find_http_method_if_else(wrong_method2);
}
end = clock();
cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
printf("\nif-else parsing functions took: %f\n", cpu_time_used);
printf("results\n r1=%i,r2=%i,r3=%i,r4=%i,r5=%i\n",r1,r2,r3,r4,r5);

start = clock();
for(int i=0; i < 10000; i++)
{
    r1=find_http_method_switch1(method1);
    r2=find_http_method_switch1(method2);
    r3=find_http_method_switch1(method3);
    r4=find_http_method_switch1(wrong_method1);
    r5=find_http_method_switch1(wrong_method2);
}
end = clock();
cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
printf("\nswitch1 parsing functions took: %f\n", cpu_time_used);
printf("results\n r1=%i,r2=%i,r3=%i,r4=%i,r5=%i\n",r1,r2,r3,r4,r5);
}
