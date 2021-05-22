#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void)
{
    printf("Enter the number of rows: ");
    int i; 
    scanf("%d", &i);

    printf("Enter the number of columns: ");
    int y; 
    scanf("%d", &y);

    int array[i][y];
    int rows, columns;
    int random;

    srand((unsigned)time(NULL));

    int * arr_1D = malloc(sizeof(int) * i * y);
    if (arr_1D == NULL)
        exit(-1);

    int count = 0;
    for(rows=0;rows<i;rows++)
        {
            for(columns=0;columns<y;columns++)
                {
                    random=rand()%100+1;

                    array[rows][columns] = random;
                    printf("%i\t",array[rows][columns]);
                    // The code for converting 2D to 1D array 
                    arr_1D[count++] =  array[rows][columns];
                }

            printf("\n");
        }

    for (int k = 0; k < count; ++k)
    {
        printf("%d ", arr_1D[k]);
    }

return 0;
}
