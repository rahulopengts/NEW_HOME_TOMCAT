set PATH=%PATH%;d:\jdk1.7.0_45\bin;
xcopy /S /I /E /Y D:\Home_Auto\NEW_HOME_TOMCAT\ws\openhab\build D:\Home_Auto\NEW_HOME_TOMCAT\ws\openhab\WebContent\WEB-INF
jar cvf openhab.war .
copy D:\Home_Auto\NEW_HOME_TOMCAT\buildwar\openhab.war D:\apache-tomcat-8.0.21\webapps