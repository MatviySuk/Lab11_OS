// server.cpp

// Програма мовою С++ яка виступає сервером
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
#include<chrono>
#include <vector>
#include <iostream>
#include <fstream>
#include <set>
#include <map>

#define TIME 10
#define MAXLINE 4096
using namespace std;

void savingVotes(map<int, int> &results, int voteValue);
void countVotesAndShowInConsole(map<int, int> &votes, const set<string> &allDataSet);
int getTheSmallestElementIndex(map<int, int> &votes);

int main()
{
    int fd;
    // FIFO file path
    char server[] = "/tmp/fifo.srv";
    char  currentClient[MAXLINE];
    char arr1[MAXLINE], arr2[MAXLINE];

    mkfifo(server, 0666);   // створення FIFO сервера
    cout << "Server is launched..." << endl;

    cout << "Enter the number of needed clients: ";
    int count; cin >> count; cin.get();

    cout << "waiting for clients..." << endl;

    // отримати по каналу сервера pid клієнтів і занести їх у вектор клієнтів
    vector<string> clients;
    for(int i = 0; i < count; i++)
    {
        fd = open(server, O_RDONLY);
        read(fd, arr1, MAXLINE);
        //   cout << arr1 << endl;
        snprintf(currentClient, sizeof(currentClient), "/tmp/fifo.%s", arr1);
        cout << "[" << currentClient << "]: connected" << endl;
        clients.push_back(currentClient);
        close(fd);
    }

    // надіслати клієнтам команду початку роботи та час для роботи
    cout << "Message for clients --> ";
    cin >> arr2; cin. get();
    snprintf(arr1, sizeof(arr1), "%s %d", arr2, TIME);

    for(int i = 0; i < count; i++)
    {
        fd = open(clients[i].c_str(), O_WRONLY);
        write(fd, arr1, strlen(arr1)+1);
        close(fd);
    }

    // у циклі отримати від кожного клієнта стрічку
    // та додати цю стрічку до загальної стрічки, яку потім розбити на міста
    // міста додати у set а потім вивести пронумеровані міста на екран та відправити клієнтам

    // отримати повідомлення від клієнтів
    string received;    // стрічка отриманих повідомлень від клієнтів

    fd = open(server, O_RDONLY);
    for(size_t i = 0; i < clients.size(); i++)
    {
        read(fd, arr1, MAXLINE);
        //  cout << arr1 << endl;
        received += arr1;
        memset(&arr1[0], 0, sizeof(arr1) );
    }
    close(fd);

   // розбити повідомлення на ідеї та занести їх у множину
   set<string>ideas;   // вектор отриманих ідей (без повторення)

   char * tmpArr = new char[received.length() + 1];
   strncpy(tmpArr, received.c_str(), received.length() + 1);
   //  cout << tmpArr << endl;

   // поділ за символом пробілу та занесення у вектор
   char * currentIdea = strtok(tmpArr, " ");

   do
   {
       ideas.insert(currentIdea);
       currentIdea = strtok(NULL, " ");
   }while(currentIdea != NULL);

   // сформувати стрічку для відправлення варіантів клієнтам та виведення
   // варіантів для голосуваня

   string toSend;  // накопичення номерів варіантів та їхніх значень
   toSend += "\nIdeas from clients:\n";
   set<string>::const_iterator it = ideas.begin();
   for(size_t i = 0; i < ideas.size(); i++, it++)
   {
       toSend += to_string(i + 1);
       toSend += "\t";
       toSend += *it;
       toSend += "\n";
   }

   cout << toSend << endl;

   // відправлення клієнтам списку для голосування
   for(size_t i = 0; i < clients.size(); i++)
   {
       fd = open(clients[i].c_str(), O_WRONLY);
       // Open FIFO for write only
       write(fd, toSend.c_str(), strlen(toSend.c_str()));
       close(fd);
   }

   cout << "Vote for 3 variants you like the most: <var1> <var2> <var3>" << endl;
   cout << "Waiting for votes from clients..." << endl;

   // отримати у стрічку голоси від клієнтів
   string votesStr;
   for(size_t i = 0; i < clients.size(); i++)
   {
        fd = open(server, O_RDONLY);
        read(fd, arr1, MAXLINE);
        cout << arr1 << endl;
        votesStr += arr1;
        memset(&arr1[0], 0, sizeof(arr1) );
        close(fd);
   }

    cout << "Received such votes:" << endl;
    cout << votesStr << endl;

    vector<int> votes;  // вектор голосів
    if(tmpArr) {delete [] tmpArr; tmpArr = nullptr; }

    tmpArr = new char[votesStr.length() + 1];
    strncpy(tmpArr, votesStr.c_str(), votesStr.length() + 1);

    // розбити стрічку по знаку пробілу на голоси та занести їх у вектор
    char * currentVote = strtok(tmpArr, " ");

    do
    {
        votes.push_back(atoi(currentVote)); // не забути перевести текст у число
        currentVote = strtok(NULL, " ");
    }
    while(currentVote != NULL);

    map<int, int> results;

    // занести індекси ідей у мапу і занулити 2-ге поле (к-сть голосів)
    for(size_t i = 0; i < ideas.size(); i++, it++)
    {
        results.insert({i, 0});
    }

    // цикл по кількості голосів, додати голос до потрібної ідеї
    for(size_t i = 0; i < votes.size(); i++)
    {
        savingVotes(results, votes[i]);
    }

    cout << "\n-------\nWe have counted your votes and happy to announce the results" << endl;

    sleep(1);

    // ф-я визначає ідеї з найбільшими голосами та виводить їх на екран
    countVotesAndShowInConsole(results, ideas);

    unlink(server);  // від'єднати пайп - клопоти процесу, який його створив
    cout << "end of work of the server..." << endl;
    if(tmpArr) {delete [] tmpArr; tmpArr = nullptr; }
    sleep(1);

    return 0;
}
//-----------------------------------------------------------------------------
void savingVotes(map<int, int> &results, int voteValue)
{
    map<int, int>::iterator itr;

    for(itr = results.begin(); itr != results.end(); ++itr)
    {
        if (itr->first == voteValue)
        {
            itr->second = itr->second + 1;
        }
    }
}
//-----------------------------------------------------------------------------
int getTheSmallestElementIndex(map<int, int> &votes)
{
    int index = votes.begin()->first;
    int value = votes.begin()->second;
    map<int, int>::iterator itr;

    for(itr = votes.begin(); itr != votes.end(); ++itr)
    {
        if(itr->second < value)
        {
            value = itr->second;
            index = itr->first;
        }
    }
    return index;
}
//-----------------------------------------------------------------------------
void countVotesAndShowInConsole(map<int, int> &votes, const set<string> &allDataSet)
{
    map<int, int> top;
    vector<string> allDataVector;

    set<string>::const_iterator it = allDataSet.begin();
    while (it != allDataSet.end())
    {
        allDataVector.push_back(*it);
        it++;
    }

    if (votes.size() > 2)
    {
           for(int i = 0; i < 3; i++)
           {
               top.insert({ i+1, 0 });
               top.at(i+1) = votes.at(i+1);
           }

           for(size_t i = 4; i < votes.size() ; i++)
           {
               int index = getTheSmallestElementIndex(top);
               if(top.at(index) < votes.at(i))
               {
                   top.erase(index);
                   top.insert({ i, votes.at(i) });
               }
           }

           map<int, int>::iterator itr;
           cout << "Top 3:" << endl;
           for(itr = top.begin(); itr != top.end(); itr++)
           {
               cout << allDataVector[itr->first - 1] << endl;
           }
       }
    else
    {
        cout << "There is no Top 3 options!" << endl;
    }
}
//-----------------------------------------------------------------------------
