#include<stdio.h>
#include<iostream>
#include<string.h>
using namespace std;

int check(char *s, int n, int w, int h, int len)
{
        int hl = 0, vl = 0, hb = 0;
        int i;

        i = hb + w / n;
        while (i < len)
        {
                if (s[i] != ' ')
                for ( ;s[i] != ' ' && i >= hb; i--);
                if (i < hb)
                {
                        return 0;
                }
                hb = i;
                i = hb + w / n;
                vl+=n;
                if (vl > h)
                {
                        return 0;
                }
        }
        vl+=n;
        if (vl > h)
        {
                return 0;
        }
        return 1;
}

main()
{
        int t;
        int w, h, nbytes = 3003;
        char s[1001] = {0}, line[3003], *p;
        int i, j, cnt, len, min, high, low, mid;

        cin.getline (line, 3003);
        sscanf(line, "%d", &t);

        for (j = 1; j <= t; j++)
        {
                cin.getline (line, 3003);

                sscanf(line, "%d%d", &w, &h);
                len = strlen(line);
                for (i = 0, cnt = 0; i < len; i++)
                {
                        if(line[i] == ' ')
                        {
                                cnt++;
                                if (cnt == 2)
                                {
                                        i++;
                                        break;
                                }
                        }
                }

                strcpy(s, line + i);
                min = w < h ? w : h;
                low = 0; high = min;
                len -= i;
                while (low < high)
                {
                        mid = (low + high)/2 + (low+high) % 2;
                        if (check(s, mid, w, h, len))
                        {
                                low = mid;
                        }
                        else
                        {
                                high = mid - 1;
                        }
                }
                printf("Case #%d: %d\n", j, low);
        }
}
