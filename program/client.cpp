// client.cpp

// Програма мовою С++ яка виступає клієнтом
// при комунікації процесів за допомогою
// іменоваинх каналів (named pipes) (FIFO)

// Created by Yana Luchyk, Oleksandr Shpak,
// Roman Chorniy, Matviy Suk on 08.12.2021

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include<fstream>
#include<chrono>
#include <iostream>

using namespace std;

#define MAXLINE 4096

int main()
{
    srand(time(NULL));

    int fd1;
    int fd2;

    // шлях до FIFO сервера
    char  server[] = "/tmp/fifo.srv";
    char str1[MAXLINE], str2[MAXLINE];

    int pid = getpid();

    char currentfifo[MAXLINE];
    snprintf(currentfifo, sizeof(currentfifo), "/tmp/fifo.%ld", (long) pid);
    cout << "[Client]: " << currentfifo << endl;
    mkfifo(currentfifo, 0666);

    // зчитати ідеї з файлу та покласти їх у вектор щоб потім рандомно обирати
    ifstream fin;
    fin.open("cities.txt");
    if (fin.is_open()) {
    //    cout << "Файл cities.txt успішно відкритий!" << endl;
    }
    else {
    //    cout << "Файл cities.txt не вдалося відкрити!" << endl;
       return -1;
    }

    vector<string> ideas;
    string in;
    // покласти всі ідеї у вектор
    while (!fin.eof()) {
       getline(fin, in);
       ideas.push_back(in);
    }
    fin.close();

    snprintf(str1, sizeof(str1), "%ld", (long) pid);
    // cout << str1 << endl;

    // Відправити на сервер свій pid щоб він міг сформувати адресу FIFO поточного клієнта
    fd1 = open(server, O_WRONLY);
    write(fd1, str1, strlen(str1));
    close(fd1);

    cout << "Client is launched and waiting for command from server..." << endl;

    // Отримати з сервера команду про початок роботи і час, відведений для роботи
    fd1 = open(currentfifo,O_RDONLY);
    read(fd1, str1, MAXLINE);
    close(fd1);

    // Повідомити користувачу, що ми отримали з сервера
    printf("Received command from server: %s\n", str1);

    char * ptr;
    if ((ptr = strchr(str1, ' ')) == NULL)
     {
          fprintf(stderr, "Клієнт: неправильний запит: %s\n", str1);
     }

    *ptr++ = '\0'; /* нульовий байт вставлений в str1 на місце пробілу */
                   /* ptr після збільшення (++) вказує на час для роботи */

    // повідомити час, відведений на роботу
    int timeForWork = atoi(ptr);
    cout << "------\nTime for work: " << timeForWork << endl;

    sleep(1);

    auto start = chrono::high_resolution_clock::now();
    auto end = chrono::high_resolution_clock::now();
    auto elapsed = chrono::duration_cast<chrono::seconds>(end - start);

    string sent;    // стрічка для накопичення варіантів від клієнта
    while (1)
    {
        end = chrono::high_resolution_clock::now();
        elapsed = chrono::duration_cast<chrono::seconds>(end - start);
        int randint = rand() % (ideas.size() - 1);
        //  cout << "random number = " << randint << endl;
        snprintf(str2, sizeof(str2), "%s ",  ideas.at(randint).c_str());
        sent += str2;
        cout << "[time]: " << elapsed.count() << " " << str2 << endl;

        sleep(1);

        end = chrono::high_resolution_clock::now();
        elapsed = chrono::duration_cast<chrono::seconds>(end - start);

        if(elapsed.count() >= timeForWork) break;
    }

    // надіслати сформовну стрічку варіантів на сервер
    fd1 = open(server, O_WRONLY);
    write(fd1, sent.c_str(), strlen(sent.c_str()));
    close(fd1);

    // отримати від сервера усі можливі варіанти для голосування
    fd2 = open(currentfifo,O_RDONLY);
    read(fd2, str1, MAXLINE);
    close(fd2);

    // вивести можливі варіанти для голосування на еркан
    cout << str1 << endl;

    cout << "Vote for 3 variants you like the most: <var1> <var2> <var3>" << endl;
    cout << "Enter your votes: --> ";
    // отримати голоси
    int votes[3];
    for(int i = 0; i < 3; i++)
    {
        cin >> votes[i]; cin.get();
    }

    // сформувати стрічку з голосами
    snprintf(str1, sizeof(str1), "%d %d %d ", votes[0], votes[1], votes[2] );
    cout << str1 << endl;

    // відправити голоси клієнта на сервер
    fd2 = open(server, O_WRONLY);
    write(fd2, str1, strlen(str1));
    close(fd2);

    cout << "Votes has been sent to the server" << endl;
    cout << "End of work fo the client..." << endl;

    sleep(1);
    unlink(currentfifo);    // від'єднати пайп - клопоти клієнта, який його створив

    return 0;
}
//-----------------------------------------------------------------------------
