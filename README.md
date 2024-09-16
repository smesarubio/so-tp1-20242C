# so-tp1-20242C
# Compilación e Instrucciones de Uso

## Compilación

Para compilar el proyecto, siga estos pasos:

1. Abra una terminal en el directorio del proyecto.
2. Ejecute el siguiente comando:

   ```
   make clean all
   ```

   Esto limpiará cualquier compilación anterior y compilará el proyecto.

## Ejecución

### Usando md5

Para ejecutar el programa md5, use el siguiente comando:

```
./md5 folder/*
```

Reemplace `folder/*` con la ruta a los archivos que desea procesar.

### Usando view

Hay dos formas de ejecutar el programa view:

1. Tubería directa desde md5:

   ```
   ./md5 folder/* | ./view
   ```

2. En terminales separadas:

   Terminal 1:
   ```
   ./md5 folder/*
   ```

   Terminal 2:
   ```
   ./view /shm
   ```
