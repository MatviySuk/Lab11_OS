# Lab11_OS

Команда:
• Лучик Яна
• Сук Матвій
• Шпак Олександр
• Чорний Роман

Варіант №3:

Створити програму, що моделює наступну ситуацію: Процес-науковий керівник проекту пропонує виконавців проекту-дочірні процеси. Процес-керівник створює додаток-віртуальну дошку (файл), де можна генерувати ідеї для проекту. Процеси-виконавці генерують ідеї, записуючи їх на спільну дошку. На виконання даного завдання вони мають 3 хвилини, після чого процес-керівник призупиняє їхню роботу і виводить на екран усі згенеровані ідеї, нумеруючи кожну з них. Процеси-виконавці голосують за три найкращі ідеї. Після чого процес-керівник записує на дошку три найкращі ідеї і закриває роботу додатку-віртуальної дошки, зберігаючи її вміст. Реалізувати дану модель, використовуючи пайпи (робота в межах однієї системи)

Розподіл обов'язків:

Лучик Яна:
Займається роботою пайпів для виконавця: реалізовує під'єднання до керівника та передає стрічку, яка дозволить керівникові підключатись до пайпів конкретного виконавця (для читання від нього інформації). Код: (client.cpp) 29-41, 66-94, 119-130, 145-154.

Сук Матвій:
Займається збереженням ідей, присланих виконавцями, у список для подальшого збереження голосування. Після отриманих голосів від виконавців, занесення результатів голосування до списку. Вибір трьох найбільш популярних ідей. Код: (server.cpp) 164-183, 193-264.

Шпак Олександр:
Займається роботою пайпів для сервера: створює пайп для відправки повідомлень виконавцям, організовує все спілкування керівника з виконавцями, включаючи перше очікування на підключення з подальшою відправкою повідомлень виконавцям про початок роботи, дії зі списком ідей(приймання, опрацювання та відправлення виконавцям для голосування). Код:(server.cpp) 33-162, 185-188. 

Чорний Роман:
Займається реалізацією генерації ідей, обмежуючи час виконавців, та голосуванням за три 3 ідей з усіх варіантів, присланих керівником. Код:(client.cpp) 43-63, 96-117, 132-143.

