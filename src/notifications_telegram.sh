BOT_TOKEN=6843343341:AAHIy6QnMnymTnL04E8cOJKffeJ7LglYTJM
CHAT_WITH_USER_ID=820212262
TIME="10"
if [ "$CI_JOB_STATUS" == "success" ]; then
  MESSAGE="Done ✅"
else
  MESSAGE="Error 🚫"
fi
URL="https://api.telegram.org/bot$BOT_TOKEN/sendMessage"
TEXT="Deploy status: "${MESSAGE}" %0A%0AProject:+$CI_PROJECT_NAME%0AURL:+$CI_PROJECT_URL/pipelines/$CI_PIPELINE_ID/%0ABranch:+$CI_COMMIT_REF_SLUG"

curl -s --max-time $TIME -d "chat_id=$CHAT_WITH_USER_ID&disable_web_page_preview=1&text=$TEXT" $URL > /dev/null -d chat_id=${CHAT_WITH_USER_ID} 
