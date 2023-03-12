#include <iostream>
#include <pthread.h>
#include <vector>
#include <map>
#include <fstream>
#include <string>
#include <algorithm>
#include <cmath>

using namespace std;

struct s{
    map<int, vector<int>> powers;
    int mappers, reducers, file_no;
    vector<string> file_names;
    pthread_barrier_t barr;
    pthread_mutex_t writeLock;
};

struct ids{
    s* aux;
    int thread_idM, thread_idR;
};

int min(int a, int b){
    if(a < b)
        return a;
    return b;
}

double new_pow(double a, int b)
{
    double result = 1;
    while(b > 0)
    {
        if(b & 1)
            result = (result * a);
        a *= a;
        b /= 2;
    }
    return result;
}

double newton_wrap(int n, int power)
{

    double x = 2;
    while(abs(new_pow(x, power) - n) > 0.5)
        x = x - (new_pow(x, power) - n) / (power * new_pow(x, power - 1));
    return x;
}


void* map_func(void* arg)
{
    ids* id = (ids*) arg;

    int start, end;
    start = id->thread_idM * id->aux->file_no / id->aux->mappers;
    end = min((id->thread_idM + 1) * id->aux->file_no / id->aux->mappers, id->aux->file_no);

    for(int i = start; i < end; i++)
    {
        ifstream f (id->aux->file_names[i]);
        int numbers;
        f >> numbers;

        for(int j = 0; j < numbers; j++)
        {
            int nr;
            f >> nr;

            for(int k = 2; k <= id->aux->reducers + 1; k++)
            {
                float aprox = newton_wrap(nr, k);
                if(new_pow(round(aprox), k) == nr*1.0)
                {
                    pthread_mutex_lock(&id->aux->writeLock);
                    id->aux->powers[k].push_back(nr);
                    pthread_mutex_unlock(&id->aux->writeLock);
                }
            }
        }

        f.close();
    }

    pthread_barrier_wait(&id->aux->barr);
    pthread_exit(NULL);
}

void* red_func(void* arg)
{
    ids* id = (ids*) arg;
    pthread_barrier_wait(&id->aux->barr);

    int nr_power = id->thread_idR + 2;
    sort(id->aux->powers[nr_power].begin(), id->aux->powers[nr_power].end());
    id->aux->powers[nr_power].erase(unique(id->aux->powers[nr_power].begin(), id->aux->powers[nr_power].end()), id->aux->powers[nr_power].end());

    string out_file_name = "out" + to_string(nr_power) + ".txt";
    ofstream g (out_file_name);
    g << id->aux->powers[nr_power].size();    
    g.close();

    pthread_exit(NULL);
}

int main(int argc, char* argv[])
{
    s* aux = new s;

    ifstream f (argv[3]);
    int read_files_no;
    f >> read_files_no;
    aux->file_no = read_files_no;
    for(int i = 0; i < read_files_no; i++)
    {
        string file_name;
        f >> file_name;
        aux->file_names.push_back(file_name);
    }
    f.close();

    aux->mappers = stoi (argv[1]);
    aux->reducers = stoi (argv[2]);

    ids* id = new ids[aux->mappers + aux->reducers];

    pthread_t threadsM[aux->mappers];
    pthread_t threadsR[aux->reducers];

    pthread_barrier_init (&aux->barr, NULL, aux->mappers + aux->reducers);
    pthread_mutex_init (&aux->writeLock, NULL);

    for(int i = 0; i < aux->mappers; i++)
    {
        id[i].thread_idM = i;
        id[i].aux = aux;
    }

    for(int i = 0; i < aux->reducers; i++)
    {
        id[i].thread_idR = i;
        id[i].aux = aux;
    }

    for(int i = 0; i < aux->mappers; i++)
    {
        int r = pthread_create(&threadsM[i], NULL, map_func, &id[i]);
        if (r)
        {
            printf("Eroare la crearea thread-ului pt mapper %d\n", i);
            exit(-1);
        }
    }
    for(int i = 0; i < aux->reducers; i++)
    {
        int r = pthread_create(&threadsR[i], NULL, red_func, &id[i]);
        if (r)
        {
            printf("Eroare la crearea thread-ului pt mapper %d\n", i);
            exit(-1);
        }
    }

    for(int i = 0; i < aux->reducers; i++)
    {
        int r = pthread_join(threadsR[i], NULL);
        if (r) {
            printf("Eroare la asteptarea thread-ului %d\n", i);
            exit(-1);
        }
    }

    for(int i = 0; i < aux->mappers; i++)
    {
        int r = pthread_join(threadsM[i], NULL);
        if (r) {
            printf("Eroare la asteptarea thread-ului %d\n", i);
            exit(-1);
        }
    }

    pthread_barrier_destroy(&aux->barr);
    pthread_mutex_destroy(&aux->writeLock);
    delete aux;
    delete[] id;
    return 0;
}