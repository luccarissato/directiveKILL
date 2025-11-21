# directiveKILL

Directive: KILL é um fixed-shooter arcade com elementos de bullet-hell fortemente inspirados em jogos retrô como atari e outros jogos de época!

Este repositório contém o código-fonte, assets e um `Makefile` para Linux. O objetivo deste README é permitir que qualquer pessoa que clone o repo consiga compilar e executar o jogo em Linux (e dar orientações rápidas para Windows).

**Requisitos**
- GCC (ou clang) e `make`
- `pkg-config`
- Raylib (biblioteca C). Preferível instalar pela sua distro ou compilar e instalar em `/usr/local`.

**Build — Linux (recomendado)**
1. Instale dependências de compilação (exemplo Ubuntu/Debian):
```bash
sudo apt update
sudo apt install build-essential pkg-config cmake git
```
2. Instale Raylib (duas opções):
- via pacote da sua distribuição (se disponível):
	- Ex.: `sudo apt install libraylib-dev` (nem sempre disponível em todas as distros)
- compilando e instalando a partir do código fonte (recomendado quando não existir pacote):
```bash
git clone https://github.com/raysan5/raylib.git
cd raylib
mkdir build && cd build
cmake .. -DBUILD_SHARED_LIBS=ON -DPLATFORM=Desktop
make -j$(nproc)
sudo make install
```
3. (Opcional) Se você instalou Raylib num prefixo customizado, configure `PKG_CONFIG_PATH` antes de construir o jogo. Ex.:
```bash
export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH
```
4. No diretório do projeto (onde está este `README.md`), rode:
```bash
make clean
make -j$(nproc)
make check   # útil para ver o que o Makefile detectou
make run     # ou: ./main
```

Observações sobre o `Makefile` do projeto:
- O `Makefile` usa `pkg-config --cflags/--libs raylib` por padrão. Se `pkg-config` não encontrar `raylib`, o `make` reportará uma mensagem com instruções.
- O `Makefile` embute um `rpath` no binário quando possível (usa `pkg-config` para obter `libdir`) — isso ajuda a tornar o executável executável por outras pessoas sem passos adicionais.
- Se você não quiser instalar Raylib globalmente, passe `RAYLIB_CFLAGS` e `RAYLIB_LIBS` ao `make`:
```bash
make RAYLIB_CFLAGS="-I/path/to/raylib/include" RAYLIB_LIBS="-L/path/to/raylib/lib -lraylib -lm"
```

**Build — Windows (MSYS2 / PowerShell)**
- Há um script de build em PowerShell no repositório (`scripts/build.ps1`) usado durante o desenvolvimento no Windows.
- Recomenda-se usar MSYS2/MinGW ou Visual Studio + vcpkg para instalar raylib no Windows. A documentação do Raylib tem instruções específicas.

**Executando**
- `make run` — compila (se necessário) e executa o binário.
- Se o loader reclamar que não encontra `libraylib.so`, você tem três opções:
	1. Reexecutar `make run` (o `Makefile` tenta usar o `libdir` detectado automaticamente).
	2. Registrar `/usr/local/lib` (ou outro prefixo) no cache do linker (recomendado para sistemas multiusuário):
		 ```bash
		 echo '/usr/local/lib' | sudo tee /etc/ld.so.conf.d/usr_local_lib.conf
		 sudo ldconfig
		 ldconfig -p | grep raylib
		 ```
	3. Exportar `LD_LIBRARY_PATH` temporariamente:
		 ```bash
		 export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
		 ./main
		 ```

**Solução de problemas comuns**
- `pkg-config: Package 'raylib' not found`:
	- Verifique se `raylib.pc` está em `/usr/lib/pkgconfig` ou `/usr/local/lib/pkgconfig` e se `PKG_CONFIG_PATH` inclui esse diretório.
- `undefined reference to 'atan2f'` ou erros sobre `libm`:
	- O `Makefile` já adiciona `-lm` por padrão; se você usar overrides, certifique-se de incluir `-lm` nas suas `RAYLIB_LIBS`.
- `error while loading shared libraries: libraylib.so.*: cannot open shared object file`:
	- Execute os passos de `ldconfig` acima ou rode com `LD_LIBRARY_PATH` temporário.
- Problemas com timestamps ("modification time in the future"):
	- Ajuste o relógio do sistema ou ignore; isso é um aviso de `make` quando o arquivo tem timestamp maior que o relógio atual.

**Contribuição**
- Pull requests são bem-vindos. Por favor descreva mudanças no código e atualize este README se adicionar novas dependências ou passos de build.
