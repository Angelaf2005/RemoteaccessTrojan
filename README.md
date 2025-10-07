# Proyecto: Mini‑RAT en C++: "RemoteaccessTrojan"

> ⚠️ **Aviso importante (uso educativo)**  
> Este repositorio contiene código con funcionalidades capaces de controlar remotamente un equipo.  
> **Solo** ejecutar en entornos de laboratorio controlados (máquinas/VMs propias) o con autorización expresa.  
> No uses este código en equipos de terceros ni redes públicas.

---

## Descripción general
Cliente/servidor minimalista para control remoto cifrado en Windows. El cliente se conecta a un servidor, negocia AES-128 y ejecuta comandos remotos con destino a un equipo Windows.
El sistema se compone de cuatro módulos principales: 
1) Comm: Se encarga de establecer comunicaciones entre cliente y servidor usando Winsock2, gestionando conexión TCP.
2) Exec:Procesa los comandos del servidor y devuelve la salida del comando de forma cifrada  
3) Criptografía: Mediante AES 128 CBC se cifra y decifran los mensajes, uso de Libsodium para intercambiar de forma segura la clave AES entre cliente y servidor.
4) Persistencia:Implementa una copia del ejecutable hacia la carpeta del usuario %APPDATA%, crear un registro que permite ejecutar el código, y también permite borrar la persistencia de forma remota.  

---
## Integrantes
- Angel Adrian Alvarez Flores — Módulo Comm
- Abraham Alejandro Carreon Soriano — Módulo Exec
- Jesus Kenneth Maurizio Martinez Vazquez  — Módulo Crypto
- Raul Alejandro Rios Tururbiates — Módulo Persist
---
## 🛠 Requisitos
- Windows 10 / 11 (x64)  
- Visual Studio 2019 o superior  
- **OpenSSL** (headers + libs) — para AES (EVP)  
- **Libsodium** (headers + libs) — para intercambio asimétrico tipo `crypto_box_seal`  
- Winsock2 (Windows SDK)

---
## Compilación:
Ejecución del cliente:
Ejecución del servidor (Python de ejemplo):

---
## Formato de mensajes AES (CBC) 
• Longitud total del payload: 4 bytes
• Vector de inicialización (IV): 16 bytes
• Datos cifrados (ciphertext): Bloques de 16 bytes.0

---

## ⚠️ Consideraciones de Seguridad
🔒 Uso Ético
Solo usar en redes propias o con autorización explícita
Respetar términos de servicio de proveedores
Propósito educativo/administrativo únicamente
🛡️ Detección
Los comportamientos del programa pueden levantar levantar la alarma de los antivirus

---

## Referencias
• Documentación OpenSSL AES
• Winsock2 MSDN
• API de Registro de Windows
• Libsodium 

---

## 📄 Licencia
Este proyecto está bajo la MIT License - consulta el archivo LICENSE para más detalles.

---
