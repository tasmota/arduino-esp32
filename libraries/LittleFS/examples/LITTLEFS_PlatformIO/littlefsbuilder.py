Import("env")
print("Replace MKSPIFFSTOOL with mklittkefs")
env.Replace( MKSPIFFSTOOL=env.get("PROJECT_DIR") + '/mklittlefs' )