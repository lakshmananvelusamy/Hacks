#include<stdio.h>
#include<iostream>
#include<string.h>
using namespace std;

main()
{
        int t, i, j, min, minindex;
        char line[1001];
        int len;

        cin.getline (line, 1001);
        sscanf(line, "%d", &t);

        for (i = 1; i <= t; i++)
        {
                int cnt[8] = {0};
                cin.getline (line, 1001);
                len = strlen(line);

                for (j = 0; j < len; j++)
                {
                        switch (line[j])
                        {
                                case 'H':
                                        cnt[0]++;
                                        break;
                                case 'A':
                                        cnt[1]++;
                                        break;
                                case 'C':
                                        cnt[2]++;
                                        break;
                                case 'K':
                                        cnt[3]++;
                                        break;
                                case 'E':
                                        cnt[4]++;
                                        break;
                                case 'R':
                                        cnt[5]++;
                                        break;
                                case 'U':
                                        cnt[6]++;
                                        break;
                                case 'P':
                                        cnt[7]++;
                                        break;
                        }
                }

                min = 1001;
                for (j = 0; j <= 7; j++)
                {
                        if (cnt[j] < min)
                        {
                                min = cnt[j];
                                minindex = j;
                        }
                }
                if (minindex == 2)
                {
                        printf("Case %d: %d\n", i, cnt[2] / 2);
                }
                else if (cnt[2] >= min * 2)
                {
                        printf("Case %d: %d\n", i, min);
                }
                else
                {
                        printf("Case %d: %d\n", i, cnt[2] / 2);
                }
        }
}