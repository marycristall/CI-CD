# Basic CI/CD  

## Part 1. Настройка gitlab-runner  

**Подними виртуальную машину Ubuntu Server 22.04 LTS.** 
Будь готов, что в конце проекта нужно будет сохранить дамп образа виртуальной машины.  

**Скачай и установи на виртуальную машину gitlab-runner.**  
Длинной командой curl -L "https://packages.gitlab.com/install/repositories/runner/gitlab-runner/script.deb.sh" | sudo bash  настраиваем репозиторий 
![alt text](</src/screens/Снимок экрана 2024-05-04 в 03.03.08.png>)   

Затем, устанавливаем gitlab-runner командой sudo apt install gitlab-runner  

![alt text](</src/screens/Снимок экрана 2024-05-04 в 03.05.40.png>)

**Запусти gitlab-runner и зарегистрируй его для использования в текущем проекте (DO6_CICD).**  
 
Для этого регистрируем свой гитлаб-раннер в гитлаб командой sudo gitlab-runner register, все данные находятся на платформе сбера под ссылкой на репозиторий, название заполняем, остальное по желанию  
![alt text](</src/screens/Снимок экрана 2024-05-04 в 03.10.23.png>)  

После регистрации проверяем, что гитлаб-раннер запустился и работает  

![alt text](</src/screens/Снимок экрана 2024-05-04 в 03.11.40.png>)  

## Part 2. Сборка  

**Напиши этап для CI по сборке приложений из проекта C2_SimpleBashUtils.**  

Для этого, первым делом, склонила себе на компьютер текущий проект, создала рабочую ветку и скопировала из проекта СимплБаш папки cat и grep в src. 
Создала файл gitlab-ci.yml и прописала там стадию сборки, что нужно, чтобы ее сделать и куда сохранить артифакты (исполняемые файлы) и в течение какого времени:  

```
default:
  tags: [build_tag]

stages:
  - build

job_build:
  stage: build
  tags:
    - build_tag
  script:
  - cd src/cat
  - make
  - cd ../grep
  - make
  artifacts:
    paths:
    - src/cat/
    - src/grep/
    expire_in: 30 day
```
Проверяем, что все работает - пушим в репозиторий и смотрим пайплайн:  

![alt text](</src/screens/Снимок экрана 2024-05-07 в 18.41.40.png>)  

Видим, что собранные артифакты доступны к скачиванию:  
  
![alt text](</src/screens/Снимок экрана 2024-05-07 в 18.45.09.png>)  

## Part 3. Тест кодстайла  

**Напиши этап для CI, который запускает скрипт кодстайла (clang-format).**
Если кодстайл не прошел, то «зафейли» пайплайн.
В пайплайне отобрази вывод утилиты clang-format.  

Для этого задания по образу и подобию первой стадии пишем вторую, тестируя кланг-формат. 


```
job_style:
  stage: style
  tags:
    - style_tag
  script:
  - cd src/cat
  - clang-format -n -Werror *.c *.h
  - cd ../grep
  - clang-format -n -Werror *.c *.h
  ``` 

Флаг -Werror нужен для того, чтобы при возникновении ошибок возникало не предупреждение, а ошибка, и в пайплайне появлялся фейл, иначе проверка пройдет успешно, несмотря на результаты вывода.

Нельзя забывать, что на машине с раннером должен быть установлен кланг-формат:  

![alt text](</src/screens/Снимок экрана 2024-05-04 в 03.56.48.png>)  

Пушим и смотрим пайплайн - на скрине результаты с ошибкой и без:  

![alt text](</src/screens/Снимок экрана 2024-05-07 в 18.46.38.png>)  

## Part 4. Интеграционные тесты  

Прописываем стадию тестов, которая выполнится только при условии успешного выполнения предыдущих стадий:   
```
job_test:
  stage: test
  when: on_success
  tags:
    - test_tag
  script:
  - cd src/cat
  - make
  - echo "Beginning tests for cat"
  - make testing
  - cd ../grep
  - make
  - echo "Beginning tests for grep"
  - sh s21_test_grep.sh
```
Пуш и смотрим пайплайн:  
![alt text](</src/screens/Снимок экрана 2024-05-07 в 18.46.38-1.png>)  

## Part 5. Этап деплоя  

Создала вторую виртуальную машину, в итоге есть две ws1 и ws2. На ws1 стоит и запущен гитлаб-раннер, на ней будет разворачиваться и тестироваться код (откуда бы мы не пушили), а на вторую уже потом будут переноситься исполняемые файлы. 
Чтобы это было возможно, между ними следует настроить статическую маршрутизацию:  

![alt text](</src/screens/Снимок экрана 2024-05-07 в 13.54.00.png>)  

Затем алгоритм действий был следующий:  
- в машине с раннером ws1 сменила пользователя sudo su gitlab-runner  
- сгенерировала ключ 
- добавила этот ключ во вторую машину ws2 
- обратно сменила пользователя  
  
![alt text](</src/screens/Снимок экрана 2024-05-07 в 15.02.37.png>)  

На ws2 дала права на папку /usr/local/bin/  

![alt text](</src/screens/Снимок экрана 2024-05-07 в 15.40.51.png>)  

Скрипт, который будет копировать и перемещать исполняемые файлы:  

```
#!/bin/bash

HOST="172.24.116.8"
USER="w"
TMP_DIR="/home/w"
DEST_DIR="/usr/local/bin"

scp -r src/cat $USER@$HOST:$TMP_DIR
scp -r src/grep/ $USER@$HOST:$TMP_DIR

if [ $? -eq 0 ]; then
    echo "Artifacts copied to ws2"
else
    echo "Error: artifacts didn't copy to ws2"
    exit 1
fi

ssh $USER@$HOST "
				  rm -rf /usr/local/bin/cat; 
				  rm -rf /usr/local/bin/grep;
				  mv $TMP_DIR/cat $DEST_DIR; 
				  mv $TMP_DIR/grep $DEST_DIR
				" 


if [ $? -eq 0 ]; then
    echo "Files moved to folder bin successfully"
else
    echo "Error: files didn't move to folder bin"
    exit 1
fi
```
Стадия деплоя, по заданию запуск ищет вручную:  

```
deploy:
  stage: deploy
  when: manual
  tags:
    - deploy_tag
  script:
    - sh ./src/script_for_deploy.sh
```

![alt text](</src/screens/Снимок экрана 2024-05-07 в 18.50.59.png>)  

Ход выполнения также можно развернуть:  

![alt text](</src/screens/Снимок экрана 2024-05-07 в 19.15.39.png>)  

В результате ты должен получить готовые к работе приложения из проекта C2_SimpleBashUtils (s21_cat и s21_grep) на второй виртуальной машине.  

![alt text](</src/screens/Снимок экрана 2024-05-07 в 19.19.06.png>)  

## Part 6. Дополнительно. Уведомления  

Открываем телеграм и находим бота BotFather, следуем инструкциям     

![alt text](</src/screens/Снимок экрана 2024-05-07 в 19.23.14.png>)  

Следом нужно выяснить свой айди юзера, чтобы бот мог отправлять сообщения. Находим своего бота, щлем ему любое сообщение, после через браузер открываем ссылку https://api.telegram.org/bot%TOKEN%/getUpdates , заменив %TOKEN% на токен, выданный ботом.

Выводится такая строка, нужен выделенный номер:  

![alt text](</src/screens/Снимок экрана 2024-05-07 в 19.27.22.png>)  

Дальше пишем скрипт, используя данные в материалс:  

```
BOT_TOKEN="токен"
CHAT_WITH_USER_ID="айди"
TIME="10"
if [ "$CI_JOB_STATUS" == "success" ]; then
  MESSAGE="Done ✅"
else
  MESSAGE="Error 🚫"
fi
URL="https://api.telegram.org/bot$BOT_TOKEN/sendMessage"
TEXT="Deploy status: "${MESSAGE}" %0A%0AProject:+$CI_PROJECT_NAME%0AURL:+$CI_PROJECT_URL/pipelines/$CI_PIPELINE_ID/%0ABranch:+$CI_COMMIT_REF_SLUG"

curl -s --max-time $TIME -d "chat_id=$CHAT_WITH_USER_ID&disable_web_page_preview=1&text=$TEXT" $URL > /dev/null -d chat_id=${CHAT_WITH_USER_ID} 
```

Прописываем запуск скрипта после выполнения деплоя:  
```
deploy:
  stage: deploy
  when: manual
  tags:
    - deploy_tag
  script:
    - sh ./src/script_for_deploy.sh
  after_script:
    - ./src/notifications_telegram.sh CI  
```

По итогу при выполнении деплоя приходят сообщения через бот с отчетом о результате:  

![alt text](</src/screens/Снимок экрана 2024-05-07 в 19.32.13.png>)  

![alt text](</src/screens/Снимок экрана 2024-05-07 в 19.39.44.png>)  

![alt text](</src/screens/Снимок экрана 2024-05-07 в 19.39.58.png>)