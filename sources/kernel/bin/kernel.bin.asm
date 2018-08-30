
sources/kernel/bin/kernel.bin:     format de fichier elf32-i386


Déassemblage de la section .text :

00100000 <__start>:
  100000:	02 b0 ad 1b 03 00    	add    dh,BYTE PTR [eax+0x31bad]
  100006:	00 00                	add    BYTE PTR [eax],al
  100008:	fb                   	sti    
  100009:	4f                   	dec    edi
  10000a:	52                   	push   edx
  10000b:	e4 66                	in     al,0x66
  10000d:	90                   	nop
  10000e:	66 90                	xchg   ax,ax

00100010 <filesystem_setup>:
directory_t *root = NULL;

directory_t *alloc_directorie(const char *name);

void filesystem_setup()
{
  100010:	53                   	push   ebx
  100011:	83 ec 14             	sub    esp,0x14
    log("Allocating root directorie...");
  100014:	68 80 1e 11 00       	push   0x111e80
  100019:	e8 82 16 00 00       	call   1016a0 <log>

/* === Directories ========================================================== */

directory_t *alloc_directorie(const char *name)
{
    directory_t *dir = MALLOC(directory_t);
  10001e:	c7 04 24 8c 00 00 00 	mov    DWORD PTR [esp],0x8c
  100025:	e8 76 6f 00 00       	call   106fa0 <malloc>

    dir->name[0] = '\0';
    strncpy((char *)&dir->name, name, PATH_FILE_NAME_SIZE);
  10002a:	83 c4 0c             	add    esp,0xc
    dir->name[0] = '\0';
  10002d:	c6 00 00             	mov    BYTE PTR [eax],0x0
    directory_t *dir = MALLOC(directory_t);
  100030:	89 c3                	mov    ebx,eax
    strncpy((char *)&dir->name, name, PATH_FILE_NAME_SIZE);
  100032:	68 80 00 00 00       	push   0x80
  100037:	68 9e 1e 11 00       	push   0x111e9e
  10003c:	50                   	push   eax
  10003d:	e8 fe 5d 00 00       	call   105e40 <strncpy>
    dir->directories = sll_new();
  100042:	e8 99 7b 00 00       	call   107be0 <sll_new>
  100047:	89 83 84 00 00 00    	mov    DWORD PTR [ebx+0x84],eax
    dir->files = sll_new();
  10004d:	e8 8e 7b 00 00       	call   107be0 <sll_new>
    root = alloc_directorie("ROOT");
  100052:	89 1d 00 ae 23 00    	mov    DWORD PTR ds:0x23ae00,ebx
    dir->files = sll_new();
  100058:	89 83 80 00 00 00    	mov    DWORD PTR [ebx+0x80],eax
}
  10005e:	83 c4 18             	add    esp,0x18
  100061:	5b                   	pop    ebx
  100062:	c3                   	ret    
  100063:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
  100069:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00100070 <filesystem_get_directory>:
{
  100070:	55                   	push   ebp
  100071:	57                   	push   edi
  100072:	56                   	push   esi
  100073:	53                   	push   ebx
  100074:	81 ec 9c 00 00 00    	sub    esp,0x9c
  10007a:	8b 9c 24 b0 00 00 00 	mov    ebx,DWORD PTR [esp+0xb0]
  100081:	8d 7c 24 10          	lea    edi,[esp+0x10]
    directory_t * current = relative ? relative : root;
  100085:	85 db                	test   ebx,ebx
  100087:	0f 44 1d 00 ae 23 00 	cmove  ebx,DWORD PTR ds:0x23ae00
                    current = d;
  10008e:	31 f6                	xor    esi,esi
  100090:	eb 0f                	jmp    1000a1 <filesystem_get_directory+0x31>
  100092:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
            current = current->parent;
  100098:	8b 9b 88 00 00 00    	mov    ebx,DWORD PTR [ebx+0x88]
    for (int i = 0; path_read(path, i, buffer); i++) 
  10009e:	83 c6 01             	add    esi,0x1
  1000a1:	83 ec 04             	sub    esp,0x4
  1000a4:	57                   	push   edi
  1000a5:	56                   	push   esi
  1000a6:	ff b4 24 c0 00 00 00 	push   DWORD PTR [esp+0xc0]
  1000ad:	e8 6e 82 00 00       	call   108320 <path_read>
  1000b2:	83 c4 10             	add    esp,0x10
  1000b5:	85 c0                	test   eax,eax
  1000b7:	74 67                	je     100120 <filesystem_get_directory+0xb0>
        if (strcmp(buffer, "..") == 0)
  1000b9:	83 ec 08             	sub    esp,0x8
  1000bc:	68 a0 21 11 00       	push   0x1121a0
  1000c1:	57                   	push   edi
  1000c2:	e8 b9 5a 00 00       	call   105b80 <strcmp>
  1000c7:	83 c4 10             	add    esp,0x10
  1000ca:	85 c0                	test   eax,eax
  1000cc:	74 ca                	je     100098 <filesystem_get_directory+0x28>
        else if (strcmp(buffer, ".") == 0)
  1000ce:	83 ec 08             	sub    esp,0x8
  1000d1:	68 eb 1e 11 00       	push   0x111eeb
  1000d6:	57                   	push   edi
  1000d7:	e8 a4 5a 00 00       	call   105b80 <strcmp>
  1000dc:	83 c4 10             	add    esp,0x10
  1000df:	85 c0                	test   eax,eax
  1000e1:	74 bb                	je     10009e <filesystem_get_directory+0x2e>
            SLL_FOREARCH(i, current->directories)
  1000e3:	8b 83 84 00 00 00    	mov    eax,DWORD PTR [ebx+0x84]
  1000e9:	8b 68 04             	mov    ebp,DWORD PTR [eax+0x4]
  1000ec:	85 ed                	test   ebp,ebp
  1000ee:	74 ae                	je     10009e <filesystem_get_directory+0x2e>
                directory_t *d = (directory_t *)i->data;
  1000f0:	8b 4d 00             	mov    ecx,DWORD PTR [ebp+0x0]
                if (strcmp(buffer, d->name) == 0)
  1000f3:	83 ec 08             	sub    esp,0x8
  1000f6:	51                   	push   ecx
  1000f7:	89 4c 24 18          	mov    DWORD PTR [esp+0x18],ecx
  1000fb:	57                   	push   edi
  1000fc:	e8 7f 5a 00 00       	call   105b80 <strcmp>
  100101:	83 c4 10             	add    esp,0x10
                    current = d;
  100104:	8b 4c 24 0c          	mov    ecx,DWORD PTR [esp+0xc]
            SLL_FOREARCH(i, current->directories)
  100108:	8b 6d 04             	mov    ebp,DWORD PTR [ebp+0x4]
                    current = d;
  10010b:	85 c0                	test   eax,eax
  10010d:	0f 44 d9             	cmove  ebx,ecx
            SLL_FOREARCH(i, current->directories)
  100110:	85 ed                	test   ebp,ebp
  100112:	75 dc                	jne    1000f0 <filesystem_get_directory+0x80>
  100114:	eb 88                	jmp    10009e <filesystem_get_directory+0x2e>
  100116:	8d 76 00             	lea    esi,[esi+0x0]
  100119:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
}
  100120:	81 c4 9c 00 00 00    	add    esp,0x9c
  100126:	89 d8                	mov    eax,ebx
  100128:	5b                   	pop    ebx
  100129:	5e                   	pop    esi
  10012a:	5f                   	pop    edi
  10012b:	5d                   	pop    ebp
  10012c:	c3                   	ret    
  10012d:	8d 76 00             	lea    esi,[esi+0x0]

00100130 <filesystem_get_file>:
{
  100130:	55                   	push   ebp
  100131:	57                   	push   edi
  100132:	56                   	push   esi
  100133:	53                   	push   ebx
  100134:	81 ec 28 01 00 00    	sub    esp,0x128
  10013a:	8b b4 24 40 01 00 00 	mov    esi,DWORD PTR [esp+0x140]
  100141:	8b 9c 24 3c 01 00 00 	mov    ebx,DWORD PTR [esp+0x13c]
    char *dir_name = malloc(strlen(path));
  100148:	56                   	push   esi
  100149:	e8 72 5f 00 00       	call   1060c0 <strlen>
  10014e:	89 04 24             	mov    DWORD PTR [esp],eax
  100151:	e8 4a 6e 00 00       	call   106fa0 <malloc>
    if (path_split(path, dir_name, file_name))
  100156:	83 c4 0c             	add    esp,0xc
    char *dir_name = malloc(strlen(path));
  100159:	89 c5                	mov    ebp,eax
    if (path_split(path, dir_name, file_name))
  10015b:	8d 44 24 14          	lea    eax,[esp+0x14]
  10015f:	50                   	push   eax
  100160:	55                   	push   ebp
  100161:	56                   	push   esi
  100162:	e8 29 83 00 00       	call   108490 <path_split>
  100167:	83 c4 10             	add    esp,0x10
  10016a:	85 c0                	test   eax,eax
  10016c:	0f 84 fe 00 00 00    	je     100270 <filesystem_get_file+0x140>
    directory_t * current = relative ? relative : root;
  100172:	85 db                	test   ebx,ebx
  100174:	8d bc 24 90 00 00 00 	lea    edi,[esp+0x90]
  10017b:	0f 44 1d 00 ae 23 00 	cmove  ebx,DWORD PTR ds:0x23ae00
                    current = d;
  100182:	31 f6                	xor    esi,esi
  100184:	eb 13                	jmp    100199 <filesystem_get_file+0x69>
  100186:	8d 76 00             	lea    esi,[esi+0x0]
  100189:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
            current = current->parent;
  100190:	8b 9b 88 00 00 00    	mov    ebx,DWORD PTR [ebx+0x88]
    for (int i = 0; path_read(path, i, buffer); i++) 
  100196:	83 c6 01             	add    esi,0x1
  100199:	83 ec 04             	sub    esp,0x4
  10019c:	57                   	push   edi
  10019d:	56                   	push   esi
  10019e:	55                   	push   ebp
  10019f:	e8 7c 81 00 00       	call   108320 <path_read>
  1001a4:	83 c4 10             	add    esp,0x10
  1001a7:	85 c0                	test   eax,eax
  1001a9:	74 75                	je     100220 <filesystem_get_file+0xf0>
        if (strcmp(buffer, "..") == 0)
  1001ab:	83 ec 08             	sub    esp,0x8
  1001ae:	68 a0 21 11 00       	push   0x1121a0
  1001b3:	57                   	push   edi
  1001b4:	e8 c7 59 00 00       	call   105b80 <strcmp>
  1001b9:	83 c4 10             	add    esp,0x10
  1001bc:	85 c0                	test   eax,eax
  1001be:	74 d0                	je     100190 <filesystem_get_file+0x60>
        else if (strcmp(buffer, ".") == 0)
  1001c0:	83 ec 08             	sub    esp,0x8
  1001c3:	68 eb 1e 11 00       	push   0x111eeb
  1001c8:	57                   	push   edi
  1001c9:	e8 b2 59 00 00       	call   105b80 <strcmp>
  1001ce:	83 c4 10             	add    esp,0x10
  1001d1:	85 c0                	test   eax,eax
  1001d3:	74 c1                	je     100196 <filesystem_get_file+0x66>
            SLL_FOREARCH(i, current->directories)
  1001d5:	8b 83 84 00 00 00    	mov    eax,DWORD PTR [ebx+0x84]
  1001db:	8b 50 04             	mov    edx,DWORD PTR [eax+0x4]
  1001de:	85 d2                	test   edx,edx
  1001e0:	74 b4                	je     100196 <filesystem_get_file+0x66>
  1001e2:	89 74 24 0c          	mov    DWORD PTR [esp+0xc],esi
  1001e6:	89 6c 24 08          	mov    DWORD PTR [esp+0x8],ebp
  1001ea:	89 de                	mov    esi,ebx
  1001ec:	89 d3                	mov    ebx,edx
  1001ee:	66 90                	xchg   ax,ax
                directory_t *d = (directory_t *)i->data;
  1001f0:	8b 2b                	mov    ebp,DWORD PTR [ebx]
                if (strcmp(buffer, d->name) == 0)
  1001f2:	83 ec 08             	sub    esp,0x8
  1001f5:	55                   	push   ebp
  1001f6:	57                   	push   edi
  1001f7:	e8 84 59 00 00       	call   105b80 <strcmp>
            SLL_FOREARCH(i, current->directories)
  1001fc:	8b 5b 04             	mov    ebx,DWORD PTR [ebx+0x4]
                if (strcmp(buffer, d->name) == 0)
  1001ff:	83 c4 10             	add    esp,0x10
                    current = d;
  100202:	85 c0                	test   eax,eax
  100204:	0f 44 f5             	cmove  esi,ebp
            SLL_FOREARCH(i, current->directories)
  100207:	85 db                	test   ebx,ebx
  100209:	75 e5                	jne    1001f0 <filesystem_get_file+0xc0>
  10020b:	89 f3                	mov    ebx,esi
  10020d:	8b 6c 24 08          	mov    ebp,DWORD PTR [esp+0x8]
  100211:	8b 74 24 0c          	mov    esi,DWORD PTR [esp+0xc]
  100215:	e9 7c ff ff ff       	jmp    100196 <filesystem_get_file+0x66>
  10021a:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
        SLL_FOREARCH(i, dir->files)
  100220:	8b 83 80 00 00 00    	mov    eax,DWORD PTR [ebx+0x80]
  100226:	8b 58 04             	mov    ebx,DWORD PTR [eax+0x4]
  100229:	85 db                	test   ebx,ebx
  10022b:	74 43                	je     100270 <filesystem_get_file+0x140>
    file_t *file = NULL;
  10022d:	31 f6                	xor    esi,esi
  10022f:	90                   	nop
            file_t* f = (file_t *)i->data;
  100230:	8b 3b                	mov    edi,DWORD PTR [ebx]
            if (strcmp(file_name, f->name) == 0)
  100232:	83 ec 08             	sub    esp,0x8
  100235:	8d 47 0c             	lea    eax,[edi+0xc]
  100238:	50                   	push   eax
  100239:	8d 44 24 1c          	lea    eax,[esp+0x1c]
  10023d:	50                   	push   eax
  10023e:	e8 3d 59 00 00       	call   105b80 <strcmp>
        SLL_FOREARCH(i, dir->files)
  100243:	8b 5b 04             	mov    ebx,DWORD PTR [ebx+0x4]
            if (strcmp(file_name, f->name) == 0)
  100246:	83 c4 10             	add    esp,0x10
                file = f;
  100249:	85 c0                	test   eax,eax
  10024b:	0f 44 f7             	cmove  esi,edi
        SLL_FOREARCH(i, dir->files)
  10024e:	85 db                	test   ebx,ebx
  100250:	75 de                	jne    100230 <filesystem_get_file+0x100>
    free(dir_name);
  100252:	83 ec 0c             	sub    esp,0xc
  100255:	55                   	push   ebp
  100256:	e8 15 72 00 00       	call   107470 <free>
}
  10025b:	81 c4 2c 01 00 00    	add    esp,0x12c
  100261:	89 f0                	mov    eax,esi
  100263:	5b                   	pop    ebx
  100264:	5e                   	pop    esi
  100265:	5f                   	pop    edi
  100266:	5d                   	pop    ebp
  100267:	c3                   	ret    
  100268:	90                   	nop
  100269:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
    file_t *file = NULL;
  100270:	31 f6                	xor    esi,esi
  100272:	eb de                	jmp    100252 <filesystem_get_file+0x122>
  100274:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
  10027a:	8d bf 00 00 00 00    	lea    edi,[edi+0x0]

00100280 <dump_directorie>:
{
  100280:	55                   	push   ebp
  100281:	57                   	push   edi
  100282:	56                   	push   esi
  100283:	53                   	push   ebx
  100284:	81 ec 9c 00 00 00    	sub    esp,0x9c
    for (int i = 0; i < depth; i++)
  10028a:	8b 94 24 b4 00 00 00 	mov    edx,DWORD PTR [esp+0xb4]
{
  100291:	8b ac 24 b8 00 00 00 	mov    ebp,DWORD PTR [esp+0xb8]
    for (int i = 0; i < depth; i++)
  100298:	85 d2                	test   edx,edx
  10029a:	7e 23                	jle    1002bf <dump_directorie+0x3f>
  10029c:	8b b4 24 b4 00 00 00 	mov    esi,DWORD PTR [esp+0xb4]
  1002a3:	31 db                	xor    ebx,ebx
  1002a5:	8d 76 00             	lea    esi,[esi+0x0]
        printf("\t");
  1002a8:	83 ec 0c             	sub    esp,0xc
    for (int i = 0; i < depth; i++)
  1002ab:	83 c3 01             	add    ebx,0x1
        printf("\t");
  1002ae:	68 d6 1f 11 00       	push   0x111fd6
  1002b3:	e8 b8 63 00 00       	call   106670 <printf>
    for (int i = 0; i < depth; i++)
  1002b8:	83 c4 10             	add    esp,0x10
  1002bb:	39 de                	cmp    esi,ebx
  1002bd:	75 e9                	jne    1002a8 <dump_directorie+0x28>
    printf("%s/\n", current->name);
  1002bf:	83 ec 08             	sub    esp,0x8
  1002c2:	ff b4 24 b8 00 00 00 	push   DWORD PTR [esp+0xb8]
  1002c9:	68 a3 1e 11 00       	push   0x111ea3
  1002ce:	e8 9d 63 00 00       	call   106670 <printf>
    for (size_t i = 0; directory_get_directories(current, buffer, i); i++)
  1002d3:	83 c4 10             	add    esp,0x10
        dump_directorie(filesystem_get_directory(current, buffer), depth + 1, buffer);
  1002d6:	8b 84 24 b4 00 00 00 	mov    eax,DWORD PTR [esp+0xb4]
    for (size_t i = 0; directory_get_directories(current, buffer, i); i++)
  1002dd:	c7 44 24 04 00 00 00 	mov    DWORD PTR [esp+0x4],0x0
  1002e4:	00 
  1002e5:	8d 74 24 10          	lea    esi,[esp+0x10]
        dump_directorie(filesystem_get_directory(current, buffer), depth + 1, buffer);
  1002e9:	83 c0 01             	add    eax,0x1
  1002ec:	89 44 24 08          	mov    DWORD PTR [esp+0x8],eax

int directory_get_directories(directory_t *directory, char *name, int index)
{
    name[0] = '\0';

    SLL_FOREARCH(i, directory->directories)
  1002f0:	8b 84 24 b0 00 00 00 	mov    eax,DWORD PTR [esp+0xb0]
    name[0] = '\0';
  1002f7:	c6 45 00 00          	mov    BYTE PTR [ebp+0x0],0x0
    for (size_t i = 0; directory_get_directories(current, buffer, i); i++)
  1002fb:	8b 4c 24 04          	mov    ecx,DWORD PTR [esp+0x4]
    SLL_FOREARCH(i, directory->directories)
  1002ff:	8b 80 84 00 00 00    	mov    eax,DWORD PTR [eax+0x84]
    for (size_t i = 0; directory_get_directories(current, buffer, i); i++)
  100305:	89 ca                	mov    edx,ecx
    SLL_FOREARCH(i, directory->directories)
  100307:	8b 40 04             	mov    eax,DWORD PTR [eax+0x4]
  10030a:	85 c0                	test   eax,eax
  10030c:	74 18                	je     100326 <dump_directorie+0xa6>
    {
        if (index == 0)
  10030e:	85 c9                	test   ecx,ecx
  100310:	75 0a                	jne    10031c <dump_directorie+0x9c>
  100312:	eb 64                	jmp    100378 <dump_directorie+0xf8>
  100314:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
  100318:	85 d2                	test   edx,edx
  10031a:	74 5c                	je     100378 <dump_directorie+0xf8>
    SLL_FOREARCH(i, directory->directories)
  10031c:	8b 40 04             	mov    eax,DWORD PTR [eax+0x4]
        {
            strcpy(name, ((directory_t *)i->data)->name);
            return 1;
        }

        index--;
  10031f:	83 ea 01             	sub    edx,0x1
    SLL_FOREARCH(i, directory->directories)
  100322:	85 c0                	test   eax,eax
  100324:	75 f2                	jne    100318 <dump_directorie+0x98>
  100326:	8b 84 24 b4 00 00 00 	mov    eax,DWORD PTR [esp+0xb4]
  10032d:	31 db                	xor    ebx,ebx
  10032f:	8d 70 01             	lea    esi,[eax+0x1]
    SLL_FOREARCH(i, directory->files)
  100332:	8b 84 24 b0 00 00 00 	mov    eax,DWORD PTR [esp+0xb0]
    name[0] = '\0';
  100339:	c6 45 00 00          	mov    BYTE PTR [ebp+0x0],0x0
    for (size_t i = 0; (directory_get_files(current, buffer, i)); i++)
  10033d:	89 da                	mov    edx,ebx
    SLL_FOREARCH(i, directory->files)
  10033f:	8b 80 80 00 00 00    	mov    eax,DWORD PTR [eax+0x80]
  100345:	8b 40 04             	mov    eax,DWORD PTR [eax+0x4]
  100348:	85 c0                	test   eax,eax
  10034a:	74 1e                	je     10036a <dump_directorie+0xea>
        if (index == 0)
  10034c:	85 db                	test   ebx,ebx
  10034e:	75 10                	jne    100360 <dump_directorie+0xe0>
  100350:	e9 fb 00 00 00       	jmp    100450 <dump_directorie+0x1d0>
  100355:	8d 76 00             	lea    esi,[esi+0x0]
  100358:	85 d2                	test   edx,edx
  10035a:	0f 84 f0 00 00 00    	je     100450 <dump_directorie+0x1d0>
    SLL_FOREARCH(i, directory->files)
  100360:	8b 40 04             	mov    eax,DWORD PTR [eax+0x4]
        index--;
  100363:	83 ea 01             	sub    edx,0x1
    SLL_FOREARCH(i, directory->files)
  100366:	85 c0                	test   eax,eax
  100368:	75 ee                	jne    100358 <dump_directorie+0xd8>
}
  10036a:	81 c4 9c 00 00 00    	add    esp,0x9c
  100370:	5b                   	pop    ebx
  100371:	5e                   	pop    esi
  100372:	5f                   	pop    edi
  100373:	5d                   	pop    ebp
  100374:	c3                   	ret    
  100375:	8d 76 00             	lea    esi,[esi+0x0]
            strcpy(name, ((directory_t *)i->data)->name);
  100378:	83 ec 08             	sub    esp,0x8
  10037b:	ff 30                	push   DWORD PTR [eax]
        dump_directorie(filesystem_get_directory(current, buffer), depth + 1, buffer);
  10037d:	31 db                	xor    ebx,ebx
            strcpy(name, ((directory_t *)i->data)->name);
  10037f:	55                   	push   ebp
  100380:	e8 8b 5a 00 00       	call   105e10 <strcpy>
        dump_directorie(filesystem_get_directory(current, buffer), depth + 1, buffer);
  100385:	83 c4 10             	add    esp,0x10
  100388:	8b bc 24 b0 00 00 00 	mov    edi,DWORD PTR [esp+0xb0]
  10038f:	eb 10                	jmp    1003a1 <dump_directorie+0x121>
  100391:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
            current = current->parent;
  100398:	8b bf 88 00 00 00    	mov    edi,DWORD PTR [edi+0x88]
    for (int i = 0; path_read(path, i, buffer); i++) 
  10039e:	83 c3 01             	add    ebx,0x1
  1003a1:	83 ec 04             	sub    esp,0x4
  1003a4:	56                   	push   esi
  1003a5:	53                   	push   ebx
  1003a6:	55                   	push   ebp
  1003a7:	e8 74 7f 00 00       	call   108320 <path_read>
  1003ac:	83 c4 10             	add    esp,0x10
  1003af:	85 c0                	test   eax,eax
  1003b1:	74 7d                	je     100430 <dump_directorie+0x1b0>
        if (strcmp(buffer, "..") == 0)
  1003b3:	83 ec 08             	sub    esp,0x8
  1003b6:	68 a0 21 11 00       	push   0x1121a0
  1003bb:	56                   	push   esi
  1003bc:	e8 bf 57 00 00       	call   105b80 <strcmp>
  1003c1:	83 c4 10             	add    esp,0x10
  1003c4:	85 c0                	test   eax,eax
  1003c6:	74 d0                	je     100398 <dump_directorie+0x118>
        else if (strcmp(buffer, ".") == 0)
  1003c8:	83 ec 08             	sub    esp,0x8
  1003cb:	68 eb 1e 11 00       	push   0x111eeb
  1003d0:	56                   	push   esi
  1003d1:	e8 aa 57 00 00       	call   105b80 <strcmp>
  1003d6:	83 c4 10             	add    esp,0x10
  1003d9:	85 c0                	test   eax,eax
  1003db:	74 c1                	je     10039e <dump_directorie+0x11e>
            SLL_FOREARCH(i, current->directories)
  1003dd:	8b 87 84 00 00 00    	mov    eax,DWORD PTR [edi+0x84]
  1003e3:	8b 50 04             	mov    edx,DWORD PTR [eax+0x4]
  1003e6:	85 d2                	test   edx,edx
  1003e8:	74 b4                	je     10039e <dump_directorie+0x11e>
  1003ea:	89 5c 24 0c          	mov    DWORD PTR [esp+0xc],ebx
  1003ee:	89 ac 24 b8 00 00 00 	mov    DWORD PTR [esp+0xb8],ebp
  1003f5:	89 d3                	mov    ebx,edx
  1003f7:	89 f6                	mov    esi,esi
  1003f9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
                directory_t *d = (directory_t *)i->data;
  100400:	8b 2b                	mov    ebp,DWORD PTR [ebx]
                if (strcmp(buffer, d->name) == 0)
  100402:	83 ec 08             	sub    esp,0x8
  100405:	55                   	push   ebp
  100406:	56                   	push   esi
  100407:	e8 74 57 00 00       	call   105b80 <strcmp>
            SLL_FOREARCH(i, current->directories)
  10040c:	8b 5b 04             	mov    ebx,DWORD PTR [ebx+0x4]
                if (strcmp(buffer, d->name) == 0)
  10040f:	83 c4 10             	add    esp,0x10
                    current = d;
  100412:	85 c0                	test   eax,eax
  100414:	0f 44 fd             	cmove  edi,ebp
            SLL_FOREARCH(i, current->directories)
  100417:	85 db                	test   ebx,ebx
  100419:	75 e5                	jne    100400 <dump_directorie+0x180>
  10041b:	8b 5c 24 0c          	mov    ebx,DWORD PTR [esp+0xc]
  10041f:	8b ac 24 b8 00 00 00 	mov    ebp,DWORD PTR [esp+0xb8]
  100426:	e9 73 ff ff ff       	jmp    10039e <dump_directorie+0x11e>
  10042b:	90                   	nop
  10042c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
        dump_directorie(filesystem_get_directory(current, buffer), depth + 1, buffer);
  100430:	83 ec 04             	sub    esp,0x4
  100433:	55                   	push   ebp
  100434:	ff 74 24 10          	push   DWORD PTR [esp+0x10]
  100438:	57                   	push   edi
  100439:	e8 42 fe ff ff       	call   100280 <dump_directorie>
    for (size_t i = 0; directory_get_directories(current, buffer, i); i++)
  10043e:	83 44 24 14 01       	add    DWORD PTR [esp+0x14],0x1
  100443:	83 c4 10             	add    esp,0x10
  100446:	e9 a5 fe ff ff       	jmp    1002f0 <dump_directorie+0x70>
  10044b:	90                   	nop
  10044c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
            strcpy(name, ((file_t *)i->data)->name);
  100450:	8b 00                	mov    eax,DWORD PTR [eax]
  100452:	83 ec 08             	sub    esp,0x8
        for (int y = 0; y < depth + 1; y++)
  100455:	31 ff                	xor    edi,edi
            strcpy(name, ((file_t *)i->data)->name);
  100457:	83 c0 0c             	add    eax,0xc
  10045a:	50                   	push   eax
  10045b:	55                   	push   ebp
  10045c:	e8 af 59 00 00       	call   105e10 <strcpy>
        for (int y = 0; y < depth + 1; y++)
  100461:	83 c4 10             	add    esp,0x10
  100464:	8b 84 24 b4 00 00 00 	mov    eax,DWORD PTR [esp+0xb4]
  10046b:	85 c0                	test   eax,eax
  10046d:	78 18                	js     100487 <dump_directorie+0x207>
  10046f:	90                   	nop
            printf("\t");
  100470:	83 ec 0c             	sub    esp,0xc
        for (int y = 0; y < depth + 1; y++)
  100473:	83 c7 01             	add    edi,0x1
            printf("\t");
  100476:	68 d6 1f 11 00       	push   0x111fd6
  10047b:	e8 f0 61 00 00       	call   106670 <printf>
        for (int y = 0; y < depth + 1; y++)
  100480:	83 c4 10             	add    esp,0x10
  100483:	39 f7                	cmp    edi,esi
  100485:	75 e9                	jne    100470 <dump_directorie+0x1f0>
        printf("%s\n", buffer);
  100487:	83 ec 08             	sub    esp,0x8
    for (size_t i = 0; (directory_get_files(current, buffer, i)); i++)
  10048a:	83 c3 01             	add    ebx,0x1
        printf("%s\n", buffer);
  10048d:	55                   	push   ebp
  10048e:	68 b5 1e 11 00       	push   0x111eb5
  100493:	e8 d8 61 00 00       	call   106670 <printf>
    for (size_t i = 0; (directory_get_files(current, buffer, i)); i++)
  100498:	83 c4 10             	add    esp,0x10
  10049b:	e9 92 fe ff ff       	jmp    100332 <dump_directorie+0xb2>

001004a0 <filesystem_dump>:
{
  1004a0:	55                   	push   ebp
  1004a1:	57                   	push   edi
  1004a2:	56                   	push   esi
  1004a3:	53                   	push   ebx
  1004a4:	81 ec b8 00 00 00    	sub    esp,0xb8
    char * buffer = malloc(PATH_FILE_NAME_SIZE);
  1004aa:	68 80 00 00 00       	push   0x80
  1004af:	e8 ec 6a 00 00       	call   106fa0 <malloc>
  1004b4:	89 44 24 18          	mov    DWORD PTR [esp+0x18],eax
    directory_t * current = relative ? relative : root;
  1004b8:	83 c4 10             	add    esp,0x10
  1004bb:	8b 8c 24 c0 00 00 00 	mov    ecx,DWORD PTR [esp+0xc0]
  1004c2:	8b 84 24 c0 00 00 00 	mov    eax,DWORD PTR [esp+0xc0]
  1004c9:	8d 6c 24 20          	lea    ebp,[esp+0x20]
  1004cd:	85 c9                	test   ecx,ecx
  1004cf:	0f 44 05 00 ae 23 00 	cmove  eax,DWORD PTR ds:0x23ae00
                    current = d;
  1004d6:	31 db                	xor    ebx,ebx
    directory_t * current = relative ? relative : root;
  1004d8:	89 84 24 c0 00 00 00 	mov    DWORD PTR [esp+0xc0],eax
  1004df:	89 c6                	mov    esi,eax
  1004e1:	eb 0e                	jmp    1004f1 <filesystem_dump+0x51>
  1004e3:	90                   	nop
  1004e4:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
            current = current->parent;
  1004e8:	8b b6 88 00 00 00    	mov    esi,DWORD PTR [esi+0x88]
    for (int i = 0; path_read(path, i, buffer); i++) 
  1004ee:	83 c3 01             	add    ebx,0x1
  1004f1:	83 ec 04             	sub    esp,0x4
  1004f4:	55                   	push   ebp
  1004f5:	53                   	push   ebx
  1004f6:	ff b4 24 d0 00 00 00 	push   DWORD PTR [esp+0xd0]
  1004fd:	e8 1e 7e 00 00       	call   108320 <path_read>
  100502:	83 c4 10             	add    esp,0x10
  100505:	85 c0                	test   eax,eax
  100507:	74 67                	je     100570 <filesystem_dump+0xd0>
        if (strcmp(buffer, "..") == 0)
  100509:	83 ec 08             	sub    esp,0x8
  10050c:	68 a0 21 11 00       	push   0x1121a0
  100511:	55                   	push   ebp
  100512:	e8 69 56 00 00       	call   105b80 <strcmp>
  100517:	83 c4 10             	add    esp,0x10
  10051a:	85 c0                	test   eax,eax
  10051c:	74 ca                	je     1004e8 <filesystem_dump+0x48>
        else if (strcmp(buffer, ".") == 0)
  10051e:	83 ec 08             	sub    esp,0x8
  100521:	68 eb 1e 11 00       	push   0x111eeb
  100526:	55                   	push   ebp
  100527:	e8 54 56 00 00       	call   105b80 <strcmp>
  10052c:	83 c4 10             	add    esp,0x10
  10052f:	85 c0                	test   eax,eax
  100531:	74 bb                	je     1004ee <filesystem_dump+0x4e>
            SLL_FOREARCH(i, current->directories)
  100533:	8b 86 84 00 00 00    	mov    eax,DWORD PTR [esi+0x84]
  100539:	8b 78 04             	mov    edi,DWORD PTR [eax+0x4]
  10053c:	85 ff                	test   edi,edi
  10053e:	74 ae                	je     1004ee <filesystem_dump+0x4e>
  100540:	89 5c 24 0c          	mov    DWORD PTR [esp+0xc],ebx
  100544:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
                directory_t *d = (directory_t *)i->data;
  100548:	8b 1f                	mov    ebx,DWORD PTR [edi]
                if (strcmp(buffer, d->name) == 0)
  10054a:	83 ec 08             	sub    esp,0x8
  10054d:	53                   	push   ebx
  10054e:	55                   	push   ebp
  10054f:	e8 2c 56 00 00       	call   105b80 <strcmp>
            SLL_FOREARCH(i, current->directories)
  100554:	8b 7f 04             	mov    edi,DWORD PTR [edi+0x4]
                if (strcmp(buffer, d->name) == 0)
  100557:	83 c4 10             	add    esp,0x10
                    current = d;
  10055a:	85 c0                	test   eax,eax
  10055c:	0f 44 f3             	cmove  esi,ebx
            SLL_FOREARCH(i, current->directories)
  10055f:	85 ff                	test   edi,edi
  100561:	75 e5                	jne    100548 <filesystem_dump+0xa8>
  100563:	8b 5c 24 0c          	mov    ebx,DWORD PTR [esp+0xc]
  100567:	eb 85                	jmp    1004ee <filesystem_dump+0x4e>
  100569:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
  100570:	89 b4 24 c0 00 00 00 	mov    DWORD PTR [esp+0xc0],esi
    printf("%s/\n", current->name);
  100577:	83 ec 08             	sub    esp,0x8
  10057a:	ff b4 24 c8 00 00 00 	push   DWORD PTR [esp+0xc8]
  100581:	68 a3 1e 11 00       	push   0x111ea3
  100586:	e8 e5 60 00 00       	call   106670 <printf>
  10058b:	83 c4 10             	add    esp,0x10
    for (size_t i = 0; directory_get_directories(current, buffer, i); i++)
  10058e:	c7 44 24 10 00 00 00 	mov    DWORD PTR [esp+0x10],0x0
  100595:	00 
    name[0] = '\0';
  100596:	8b 44 24 08          	mov    eax,DWORD PTR [esp+0x8]
    for (size_t i = 0; directory_get_directories(current, buffer, i); i++)
  10059a:	8b 4c 24 10          	mov    ecx,DWORD PTR [esp+0x10]
    name[0] = '\0';
  10059e:	c6 00 00             	mov    BYTE PTR [eax],0x0
    SLL_FOREARCH(i, directory->directories)
  1005a1:	8b 84 24 c0 00 00 00 	mov    eax,DWORD PTR [esp+0xc0]
    for (size_t i = 0; directory_get_directories(current, buffer, i); i++)
  1005a8:	89 ca                	mov    edx,ecx
    SLL_FOREARCH(i, directory->directories)
  1005aa:	8b 80 84 00 00 00    	mov    eax,DWORD PTR [eax+0x84]
  1005b0:	8b 40 04             	mov    eax,DWORD PTR [eax+0x4]
  1005b3:	85 c0                	test   eax,eax
  1005b5:	74 17                	je     1005ce <filesystem_dump+0x12e>
        if (index == 0)
  1005b7:	85 c9                	test   ecx,ecx
  1005b9:	75 09                	jne    1005c4 <filesystem_dump+0x124>
  1005bb:	eb 73                	jmp    100630 <filesystem_dump+0x190>
  1005bd:	8d 76 00             	lea    esi,[esi+0x0]
  1005c0:	85 d2                	test   edx,edx
  1005c2:	74 6c                	je     100630 <filesystem_dump+0x190>
    SLL_FOREARCH(i, directory->directories)
  1005c4:	8b 40 04             	mov    eax,DWORD PTR [eax+0x4]
        index--;
  1005c7:	83 ea 01             	sub    edx,0x1
    SLL_FOREARCH(i, directory->directories)
  1005ca:	85 c0                	test   eax,eax
  1005cc:	75 f2                	jne    1005c0 <filesystem_dump+0x120>
  1005ce:	8b 74 24 08          	mov    esi,DWORD PTR [esp+0x8]
  1005d2:	8b bc 24 c0 00 00 00 	mov    edi,DWORD PTR [esp+0xc0]
  1005d9:	31 db                	xor    ebx,ebx
    name[0] = '\0';
  1005db:	c6 06 00             	mov    BYTE PTR [esi],0x0
    SLL_FOREARCH(i, directory->files)
  1005de:	8b 87 80 00 00 00    	mov    eax,DWORD PTR [edi+0x80]
    for (size_t i = 0; (directory_get_files(current, buffer, i)); i++)
  1005e4:	89 da                	mov    edx,ebx
    SLL_FOREARCH(i, directory->files)
  1005e6:	8b 40 04             	mov    eax,DWORD PTR [eax+0x4]
  1005e9:	85 c0                	test   eax,eax
  1005eb:	74 25                	je     100612 <filesystem_dump+0x172>
        if (index == 0)
  1005ed:	85 db                	test   ebx,ebx
  1005ef:	75 17                	jne    100608 <filesystem_dump+0x168>
  1005f1:	e9 8a 01 00 00       	jmp    100780 <filesystem_dump+0x2e0>
  1005f6:	8d 76 00             	lea    esi,[esi+0x0]
  1005f9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
  100600:	85 d2                	test   edx,edx
  100602:	0f 84 78 01 00 00    	je     100780 <filesystem_dump+0x2e0>
    SLL_FOREARCH(i, directory->files)
  100608:	8b 40 04             	mov    eax,DWORD PTR [eax+0x4]
        index--;
  10060b:	83 ea 01             	sub    edx,0x1
    SLL_FOREARCH(i, directory->files)
  10060e:	85 c0                	test   eax,eax
  100610:	75 ee                	jne    100600 <filesystem_dump+0x160>
    free(buffer);
  100612:	83 ec 0c             	sub    esp,0xc
  100615:	ff 74 24 14          	push   DWORD PTR [esp+0x14]
  100619:	e8 52 6e 00 00       	call   107470 <free>
}
  10061e:	81 c4 bc 00 00 00    	add    esp,0xbc
  100624:	5b                   	pop    ebx
  100625:	5e                   	pop    esi
  100626:	5f                   	pop    edi
  100627:	5d                   	pop    ebp
  100628:	c3                   	ret    
  100629:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
            strcpy(name, ((directory_t *)i->data)->name);
  100630:	83 ec 08             	sub    esp,0x8
  100633:	ff 30                	push   DWORD PTR [eax]
  100635:	31 f6                	xor    esi,esi
  100637:	ff 74 24 14          	push   DWORD PTR [esp+0x14]
  10063b:	e8 d0 57 00 00       	call   105e10 <strcpy>
  100640:	83 c4 10             	add    esp,0x10
  100643:	8b 9c 24 c0 00 00 00 	mov    ebx,DWORD PTR [esp+0xc0]
  10064a:	eb 0d                	jmp    100659 <filesystem_dump+0x1b9>
  10064c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
            current = current->parent;
  100650:	8b 9b 88 00 00 00    	mov    ebx,DWORD PTR [ebx+0x88]
    for (int i = 0; path_read(path, i, buffer); i++) 
  100656:	83 c6 01             	add    esi,0x1
  100659:	83 ec 04             	sub    esp,0x4
  10065c:	55                   	push   ebp
  10065d:	56                   	push   esi
  10065e:	ff 74 24 14          	push   DWORD PTR [esp+0x14]
  100662:	e8 b9 7c 00 00       	call   108320 <path_read>
  100667:	83 c4 10             	add    esp,0x10
  10066a:	85 c0                	test   eax,eax
  10066c:	74 6a                	je     1006d8 <filesystem_dump+0x238>
        if (strcmp(buffer, "..") == 0)
  10066e:	83 ec 08             	sub    esp,0x8
  100671:	68 a0 21 11 00       	push   0x1121a0
  100676:	55                   	push   ebp
  100677:	e8 04 55 00 00       	call   105b80 <strcmp>
  10067c:	83 c4 10             	add    esp,0x10
  10067f:	85 c0                	test   eax,eax
  100681:	74 cd                	je     100650 <filesystem_dump+0x1b0>
        else if (strcmp(buffer, ".") == 0)
  100683:	83 ec 08             	sub    esp,0x8
  100686:	68 eb 1e 11 00       	push   0x111eeb
  10068b:	55                   	push   ebp
  10068c:	e8 ef 54 00 00       	call   105b80 <strcmp>
  100691:	83 c4 10             	add    esp,0x10
  100694:	85 c0                	test   eax,eax
  100696:	74 be                	je     100656 <filesystem_dump+0x1b6>
            SLL_FOREARCH(i, current->directories)
  100698:	8b 83 84 00 00 00    	mov    eax,DWORD PTR [ebx+0x84]
  10069e:	8b 78 04             	mov    edi,DWORD PTR [eax+0x4]
  1006a1:	85 ff                	test   edi,edi
  1006a3:	74 b1                	je     100656 <filesystem_dump+0x1b6>
  1006a5:	89 74 24 0c          	mov    DWORD PTR [esp+0xc],esi
  1006a9:	89 de                	mov    esi,ebx
  1006ab:	90                   	nop
  1006ac:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
                directory_t *d = (directory_t *)i->data;
  1006b0:	8b 1f                	mov    ebx,DWORD PTR [edi]
                if (strcmp(buffer, d->name) == 0)
  1006b2:	83 ec 08             	sub    esp,0x8
  1006b5:	53                   	push   ebx
  1006b6:	55                   	push   ebp
  1006b7:	e8 c4 54 00 00       	call   105b80 <strcmp>
            SLL_FOREARCH(i, current->directories)
  1006bc:	8b 7f 04             	mov    edi,DWORD PTR [edi+0x4]
                if (strcmp(buffer, d->name) == 0)
  1006bf:	83 c4 10             	add    esp,0x10
                    current = d;
  1006c2:	85 c0                	test   eax,eax
  1006c4:	0f 44 f3             	cmove  esi,ebx
            SLL_FOREARCH(i, current->directories)
  1006c7:	85 ff                	test   edi,edi
  1006c9:	75 e5                	jne    1006b0 <filesystem_dump+0x210>
  1006cb:	89 f3                	mov    ebx,esi
  1006cd:	8b 74 24 0c          	mov    esi,DWORD PTR [esp+0xc]
  1006d1:	eb 83                	jmp    100656 <filesystem_dump+0x1b6>
  1006d3:	90                   	nop
  1006d4:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
        printf("\t");
  1006d8:	83 ec 0c             	sub    esp,0xc
  1006db:	68 d6 1f 11 00       	push   0x111fd6
  1006e0:	e8 8b 5f 00 00       	call   106670 <printf>
    printf("%s/\n", current->name);
  1006e5:	58                   	pop    eax
  1006e6:	5a                   	pop    edx
  1006e7:	53                   	push   ebx
  1006e8:	68 a3 1e 11 00       	push   0x111ea3
  1006ed:	e8 7e 5f 00 00       	call   106670 <printf>
  1006f2:	83 c4 10             	add    esp,0x10
    for (size_t i = 0; directory_get_directories(current, buffer, i); i++)
  1006f5:	89 5c 24 0c          	mov    DWORD PTR [esp+0xc],ebx
  1006f9:	c7 44 24 14 00 00 00 	mov    DWORD PTR [esp+0x14],0x0
  100700:	00 
  100701:	8b 5c 24 08          	mov    ebx,DWORD PTR [esp+0x8]
    SLL_FOREARCH(i, directory->directories)
  100705:	8b 44 24 0c          	mov    eax,DWORD PTR [esp+0xc]
    name[0] = '\0';
  100709:	c6 03 00             	mov    BYTE PTR [ebx],0x0
    for (size_t i = 0; directory_get_directories(current, buffer, i); i++)
  10070c:	8b 4c 24 14          	mov    ecx,DWORD PTR [esp+0x14]
    SLL_FOREARCH(i, directory->directories)
  100710:	8b 80 84 00 00 00    	mov    eax,DWORD PTR [eax+0x84]
    for (size_t i = 0; directory_get_directories(current, buffer, i); i++)
  100716:	89 ca                	mov    edx,ecx
    SLL_FOREARCH(i, directory->directories)
  100718:	8b 40 04             	mov    eax,DWORD PTR [eax+0x4]
  10071b:	85 c0                	test   eax,eax
  10071d:	74 23                	je     100742 <filesystem_dump+0x2a2>
        if (index == 0)
  10071f:	85 c9                	test   ecx,ecx
  100721:	75 15                	jne    100738 <filesystem_dump+0x298>
  100723:	e9 d8 00 00 00       	jmp    100800 <filesystem_dump+0x360>
  100728:	90                   	nop
  100729:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
  100730:	85 d2                	test   edx,edx
  100732:	0f 84 c8 00 00 00    	je     100800 <filesystem_dump+0x360>
    SLL_FOREARCH(i, directory->directories)
  100738:	8b 40 04             	mov    eax,DWORD PTR [eax+0x4]
        index--;
  10073b:	83 ea 01             	sub    edx,0x1
    SLL_FOREARCH(i, directory->directories)
  10073e:	85 c0                	test   eax,eax
  100740:	75 ee                	jne    100730 <filesystem_dump+0x290>
  100742:	8b 5c 24 0c          	mov    ebx,DWORD PTR [esp+0xc]
  100746:	8b 7c 24 08          	mov    edi,DWORD PTR [esp+0x8]
  10074a:	31 f6                	xor    esi,esi
    name[0] = '\0';
  10074c:	c6 07 00             	mov    BYTE PTR [edi],0x0
    SLL_FOREARCH(i, directory->files)
  10074f:	8b 83 80 00 00 00    	mov    eax,DWORD PTR [ebx+0x80]
    for (size_t i = 0; (directory_get_files(current, buffer, i)); i++)
  100755:	89 f2                	mov    edx,esi
    SLL_FOREARCH(i, directory->files)
  100757:	8b 40 04             	mov    eax,DWORD PTR [eax+0x4]
  10075a:	85 c0                	test   eax,eax
  10075c:	74 18                	je     100776 <filesystem_dump+0x2d6>
        if (index == 0)
  10075e:	85 f6                	test   esi,esi
  100760:	75 0a                	jne    10076c <filesystem_dump+0x2cc>
  100762:	eb 54                	jmp    1007b8 <filesystem_dump+0x318>
  100764:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
  100768:	85 d2                	test   edx,edx
  10076a:	74 4c                	je     1007b8 <filesystem_dump+0x318>
    SLL_FOREARCH(i, directory->files)
  10076c:	8b 40 04             	mov    eax,DWORD PTR [eax+0x4]
        index--;
  10076f:	83 ea 01             	sub    edx,0x1
    SLL_FOREARCH(i, directory->files)
  100772:	85 c0                	test   eax,eax
  100774:	75 f2                	jne    100768 <filesystem_dump+0x2c8>
    for (size_t i = 0; directory_get_directories(current, buffer, i); i++)
  100776:	83 44 24 10 01       	add    DWORD PTR [esp+0x10],0x1
  10077b:	e9 16 fe ff ff       	jmp    100596 <filesystem_dump+0xf6>
            strcpy(name, ((file_t *)i->data)->name);
  100780:	8b 00                	mov    eax,DWORD PTR [eax]
  100782:	83 ec 08             	sub    esp,0x8
    for (size_t i = 0; (directory_get_files(current, buffer, i)); i++)
  100785:	83 c3 01             	add    ebx,0x1
            strcpy(name, ((file_t *)i->data)->name);
  100788:	83 c0 0c             	add    eax,0xc
  10078b:	50                   	push   eax
  10078c:	56                   	push   esi
  10078d:	e8 7e 56 00 00       	call   105e10 <strcpy>
            printf("\t");
  100792:	c7 04 24 d6 1f 11 00 	mov    DWORD PTR [esp],0x111fd6
  100799:	e8 d2 5e 00 00       	call   106670 <printf>
        printf("%s\n", buffer);
  10079e:	58                   	pop    eax
  10079f:	5a                   	pop    edx
  1007a0:	56                   	push   esi
  1007a1:	68 b5 1e 11 00       	push   0x111eb5
  1007a6:	e8 c5 5e 00 00       	call   106670 <printf>
    for (size_t i = 0; (directory_get_files(current, buffer, i)); i++)
  1007ab:	83 c4 10             	add    esp,0x10
  1007ae:	e9 28 fe ff ff       	jmp    1005db <filesystem_dump+0x13b>
  1007b3:	90                   	nop
  1007b4:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
            strcpy(name, ((file_t *)i->data)->name);
  1007b8:	8b 00                	mov    eax,DWORD PTR [eax]
  1007ba:	83 ec 08             	sub    esp,0x8
    for (size_t i = 0; (directory_get_files(current, buffer, i)); i++)
  1007bd:	83 c6 01             	add    esi,0x1
            strcpy(name, ((file_t *)i->data)->name);
  1007c0:	83 c0 0c             	add    eax,0xc
  1007c3:	50                   	push   eax
  1007c4:	57                   	push   edi
  1007c5:	e8 46 56 00 00       	call   105e10 <strcpy>
            printf("\t");
  1007ca:	c7 04 24 d6 1f 11 00 	mov    DWORD PTR [esp],0x111fd6
  1007d1:	e8 9a 5e 00 00       	call   106670 <printf>
  1007d6:	c7 04 24 d6 1f 11 00 	mov    DWORD PTR [esp],0x111fd6
  1007dd:	e8 8e 5e 00 00       	call   106670 <printf>
        printf("%s\n", buffer);
  1007e2:	59                   	pop    ecx
  1007e3:	58                   	pop    eax
  1007e4:	57                   	push   edi
  1007e5:	68 b5 1e 11 00       	push   0x111eb5
  1007ea:	e8 81 5e 00 00       	call   106670 <printf>
    for (size_t i = 0; (directory_get_files(current, buffer, i)); i++)
  1007ef:	83 c4 10             	add    esp,0x10
  1007f2:	e9 55 ff ff ff       	jmp    10074c <filesystem_dump+0x2ac>
  1007f7:	89 f6                	mov    esi,esi
  1007f9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
            strcpy(name, ((directory_t *)i->data)->name);
  100800:	83 ec 08             	sub    esp,0x8
  100803:	ff 30                	push   DWORD PTR [eax]
  100805:	31 f6                	xor    esi,esi
  100807:	53                   	push   ebx
  100808:	e8 03 56 00 00       	call   105e10 <strcpy>
  10080d:	83 c4 10             	add    esp,0x10
  100810:	8b 7c 24 0c          	mov    edi,DWORD PTR [esp+0xc]
  100814:	eb 13                	jmp    100829 <filesystem_dump+0x389>
  100816:	8d 76 00             	lea    esi,[esi+0x0]
  100819:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
            current = current->parent;
  100820:	8b bf 88 00 00 00    	mov    edi,DWORD PTR [edi+0x88]
    for (int i = 0; path_read(path, i, buffer); i++) 
  100826:	83 c6 01             	add    esi,0x1
  100829:	83 ec 04             	sub    esp,0x4
  10082c:	55                   	push   ebp
  10082d:	56                   	push   esi
  10082e:	53                   	push   ebx
  10082f:	e8 ec 7a 00 00       	call   108320 <path_read>
  100834:	83 c4 10             	add    esp,0x10
  100837:	85 c0                	test   eax,eax
  100839:	74 75                	je     1008b0 <filesystem_dump+0x410>
        if (strcmp(buffer, "..") == 0)
  10083b:	83 ec 08             	sub    esp,0x8
  10083e:	68 a0 21 11 00       	push   0x1121a0
  100843:	55                   	push   ebp
  100844:	e8 37 53 00 00       	call   105b80 <strcmp>
  100849:	83 c4 10             	add    esp,0x10
  10084c:	85 c0                	test   eax,eax
  10084e:	74 d0                	je     100820 <filesystem_dump+0x380>
        else if (strcmp(buffer, ".") == 0)
  100850:	83 ec 08             	sub    esp,0x8
  100853:	68 eb 1e 11 00       	push   0x111eeb
  100858:	55                   	push   ebp
  100859:	e8 22 53 00 00       	call   105b80 <strcmp>
  10085e:	83 c4 10             	add    esp,0x10
  100861:	85 c0                	test   eax,eax
  100863:	74 c1                	je     100826 <filesystem_dump+0x386>
            SLL_FOREARCH(i, current->directories)
  100865:	8b 87 84 00 00 00    	mov    eax,DWORD PTR [edi+0x84]
  10086b:	8b 50 04             	mov    edx,DWORD PTR [eax+0x4]
  10086e:	85 d2                	test   edx,edx
  100870:	74 b4                	je     100826 <filesystem_dump+0x386>
  100872:	89 5c 24 18          	mov    DWORD PTR [esp+0x18],ebx
  100876:	89 74 24 1c          	mov    DWORD PTR [esp+0x1c],esi
  10087a:	89 d3                	mov    ebx,edx
  10087c:	89 fe                	mov    esi,edi
  10087e:	66 90                	xchg   ax,ax
                directory_t *d = (directory_t *)i->data;
  100880:	8b 3b                	mov    edi,DWORD PTR [ebx]
                if (strcmp(buffer, d->name) == 0)
  100882:	83 ec 08             	sub    esp,0x8
  100885:	57                   	push   edi
  100886:	55                   	push   ebp
  100887:	e8 f4 52 00 00       	call   105b80 <strcmp>
            SLL_FOREARCH(i, current->directories)
  10088c:	8b 5b 04             	mov    ebx,DWORD PTR [ebx+0x4]
                if (strcmp(buffer, d->name) == 0)
  10088f:	83 c4 10             	add    esp,0x10
                    current = d;
  100892:	85 c0                	test   eax,eax
  100894:	0f 44 f7             	cmove  esi,edi
            SLL_FOREARCH(i, current->directories)
  100897:	85 db                	test   ebx,ebx
  100899:	75 e5                	jne    100880 <filesystem_dump+0x3e0>
  10089b:	89 f7                	mov    edi,esi
  10089d:	8b 5c 24 18          	mov    ebx,DWORD PTR [esp+0x18]
  1008a1:	8b 74 24 1c          	mov    esi,DWORD PTR [esp+0x1c]
  1008a5:	e9 7c ff ff ff       	jmp    100826 <filesystem_dump+0x386>
  1008aa:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
        dump_directorie(filesystem_get_directory(current, buffer), depth + 1, buffer);
  1008b0:	83 ec 04             	sub    esp,0x4
  1008b3:	53                   	push   ebx
  1008b4:	6a 02                	push   0x2
  1008b6:	57                   	push   edi
  1008b7:	e8 c4 f9 ff ff       	call   100280 <dump_directorie>
    for (size_t i = 0; directory_get_directories(current, buffer, i); i++)
  1008bc:	83 44 24 24 01       	add    DWORD PTR [esp+0x24],0x1
  1008c1:	83 c4 10             	add    esp,0x10
  1008c4:	e9 3c fe ff ff       	jmp    100705 <filesystem_dump+0x265>
  1008c9:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]

001008d0 <alloc_directorie>:
{
  1008d0:	53                   	push   ebx
  1008d1:	83 ec 14             	sub    esp,0x14
    directory_t *dir = MALLOC(directory_t);
  1008d4:	68 8c 00 00 00       	push   0x8c
  1008d9:	e8 c2 66 00 00       	call   106fa0 <malloc>
    strncpy((char *)&dir->name, name, PATH_FILE_NAME_SIZE);
  1008de:	83 c4 0c             	add    esp,0xc
    dir->name[0] = '\0';
  1008e1:	c6 00 00             	mov    BYTE PTR [eax],0x0
    directory_t *dir = MALLOC(directory_t);
  1008e4:	89 c3                	mov    ebx,eax
    strncpy((char *)&dir->name, name, PATH_FILE_NAME_SIZE);
  1008e6:	68 80 00 00 00       	push   0x80
  1008eb:	ff 74 24 18          	push   DWORD PTR [esp+0x18]
  1008ef:	50                   	push   eax
  1008f0:	e8 4b 55 00 00       	call   105e40 <strncpy>
    dir->directories = sll_new();
  1008f5:	e8 e6 72 00 00       	call   107be0 <sll_new>
  1008fa:	89 83 84 00 00 00    	mov    DWORD PTR [ebx+0x84],eax
    dir->files = sll_new();
  100900:	e8 db 72 00 00       	call   107be0 <sll_new>
  100905:	89 83 80 00 00 00    	mov    DWORD PTR [ebx+0x80],eax
}
  10090b:	83 c4 18             	add    esp,0x18
  10090e:	89 d8                	mov    eax,ebx
  100910:	5b                   	pop    ebx
  100911:	c3                   	ret    
  100912:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
  100919:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00100920 <directory_create>:
{
  100920:	55                   	push   ebp
  100921:	57                   	push   edi
  100922:	56                   	push   esi
  100923:	53                   	push   ebx
  100924:	81 ec 28 01 00 00    	sub    esp,0x128
  10092a:	8b b4 24 40 01 00 00 	mov    esi,DWORD PTR [esp+0x140]
  100931:	8b 9c 24 3c 01 00 00 	mov    ebx,DWORD PTR [esp+0x13c]
    char *dir_path = malloc(strlen(path));
  100938:	56                   	push   esi
  100939:	e8 82 57 00 00       	call   1060c0 <strlen>
  10093e:	89 04 24             	mov    DWORD PTR [esp],eax
  100941:	e8 5a 66 00 00       	call   106fa0 <malloc>
    if (path_split(path, dir_path, dir_name))
  100946:	83 c4 0c             	add    esp,0xc
    char *dir_path = malloc(strlen(path));
  100949:	89 c5                	mov    ebp,eax
    if (path_split(path, dir_path, dir_name))
  10094b:	8d 44 24 14          	lea    eax,[esp+0x14]
  10094f:	50                   	push   eax
  100950:	55                   	push   ebp
  100951:	56                   	push   esi
  100952:	e8 39 7b 00 00       	call   108490 <path_split>
  100957:	83 c4 10             	add    esp,0x10
  10095a:	85 c0                	test   eax,eax
  10095c:	89 c6                	mov    esi,eax
  10095e:	75 20                	jne    100980 <directory_create+0x60>
    free(dir_path);
  100960:	83 ec 0c             	sub    esp,0xc
  100963:	55                   	push   ebp
  100964:	e8 07 6b 00 00       	call   107470 <free>
}
  100969:	81 c4 2c 01 00 00    	add    esp,0x12c
  10096f:	89 f0                	mov    eax,esi
  100971:	5b                   	pop    ebx
  100972:	5e                   	pop    esi
  100973:	5f                   	pop    edi
  100974:	5d                   	pop    ebp
  100975:	c3                   	ret    
  100976:	8d 76 00             	lea    esi,[esi+0x0]
  100979:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
    directory_t * current = relative ? relative : root;
  100980:	85 db                	test   ebx,ebx
  100982:	8d bc 24 90 00 00 00 	lea    edi,[esp+0x90]
  100989:	0f 44 1d 00 ae 23 00 	cmove  ebx,DWORD PTR ds:0x23ae00
                    current = d;
  100990:	31 f6                	xor    esi,esi
  100992:	eb 0d                	jmp    1009a1 <directory_create+0x81>
  100994:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
            current = current->parent;
  100998:	8b 9b 88 00 00 00    	mov    ebx,DWORD PTR [ebx+0x88]
    for (int i = 0; path_read(path, i, buffer); i++) 
  10099e:	83 c6 01             	add    esi,0x1
  1009a1:	83 ec 04             	sub    esp,0x4
  1009a4:	57                   	push   edi
  1009a5:	56                   	push   esi
  1009a6:	55                   	push   ebp
  1009a7:	e8 74 79 00 00       	call   108320 <path_read>
  1009ac:	83 c4 10             	add    esp,0x10
  1009af:	85 c0                	test   eax,eax
  1009b1:	74 7d                	je     100a30 <directory_create+0x110>
        if (strcmp(buffer, "..") == 0)
  1009b3:	83 ec 08             	sub    esp,0x8
  1009b6:	68 a0 21 11 00       	push   0x1121a0
  1009bb:	57                   	push   edi
  1009bc:	e8 bf 51 00 00       	call   105b80 <strcmp>
  1009c1:	83 c4 10             	add    esp,0x10
  1009c4:	85 c0                	test   eax,eax
  1009c6:	74 d0                	je     100998 <directory_create+0x78>
        else if (strcmp(buffer, ".") == 0)
  1009c8:	83 ec 08             	sub    esp,0x8
  1009cb:	68 eb 1e 11 00       	push   0x111eeb
  1009d0:	57                   	push   edi
  1009d1:	e8 aa 51 00 00       	call   105b80 <strcmp>
  1009d6:	83 c4 10             	add    esp,0x10
  1009d9:	85 c0                	test   eax,eax
  1009db:	74 c1                	je     10099e <directory_create+0x7e>
            SLL_FOREARCH(i, current->directories)
  1009dd:	8b 83 84 00 00 00    	mov    eax,DWORD PTR [ebx+0x84]
  1009e3:	8b 50 04             	mov    edx,DWORD PTR [eax+0x4]
  1009e6:	85 d2                	test   edx,edx
  1009e8:	74 b4                	je     10099e <directory_create+0x7e>
  1009ea:	89 74 24 0c          	mov    DWORD PTR [esp+0xc],esi
  1009ee:	89 6c 24 08          	mov    DWORD PTR [esp+0x8],ebp
  1009f2:	89 de                	mov    esi,ebx
  1009f4:	89 d3                	mov    ebx,edx
  1009f6:	8d 76 00             	lea    esi,[esi+0x0]
  1009f9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
                directory_t *d = (directory_t *)i->data;
  100a00:	8b 2b                	mov    ebp,DWORD PTR [ebx]
                if (strcmp(buffer, d->name) == 0)
  100a02:	83 ec 08             	sub    esp,0x8
  100a05:	55                   	push   ebp
  100a06:	57                   	push   edi
  100a07:	e8 74 51 00 00       	call   105b80 <strcmp>
            SLL_FOREARCH(i, current->directories)
  100a0c:	8b 5b 04             	mov    ebx,DWORD PTR [ebx+0x4]
                if (strcmp(buffer, d->name) == 0)
  100a0f:	83 c4 10             	add    esp,0x10
                    current = d;
  100a12:	85 c0                	test   eax,eax
  100a14:	0f 44 f5             	cmove  esi,ebp
            SLL_FOREARCH(i, current->directories)
  100a17:	85 db                	test   ebx,ebx
  100a19:	75 e5                	jne    100a00 <directory_create+0xe0>
  100a1b:	89 f3                	mov    ebx,esi
  100a1d:	8b 6c 24 08          	mov    ebp,DWORD PTR [esp+0x8]
  100a21:	8b 74 24 0c          	mov    esi,DWORD PTR [esp+0xc]
  100a25:	e9 74 ff ff ff       	jmp    10099e <directory_create+0x7e>
  100a2a:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
    directory_t *dir = MALLOC(directory_t);
  100a30:	83 ec 0c             	sub    esp,0xc
  100a33:	68 8c 00 00 00       	push   0x8c
  100a38:	e8 63 65 00 00       	call   106fa0 <malloc>
    strncpy((char *)&dir->name, name, PATH_FILE_NAME_SIZE);
  100a3d:	83 c4 0c             	add    esp,0xc
    dir->name[0] = '\0';
  100a40:	c6 00 00             	mov    BYTE PTR [eax],0x0
    directory_t *dir = MALLOC(directory_t);
  100a43:	89 c6                	mov    esi,eax
    strncpy((char *)&dir->name, name, PATH_FILE_NAME_SIZE);
  100a45:	68 80 00 00 00       	push   0x80
  100a4a:	8d 44 24 18          	lea    eax,[esp+0x18]
  100a4e:	50                   	push   eax
  100a4f:	56                   	push   esi
  100a50:	e8 eb 53 00 00       	call   105e40 <strncpy>
    dir->directories = sll_new();
  100a55:	e8 86 71 00 00       	call   107be0 <sll_new>
  100a5a:	89 86 84 00 00 00    	mov    DWORD PTR [esi+0x84],eax
    dir->files = sll_new();
  100a60:	e8 7b 71 00 00       	call   107be0 <sll_new>
        dir->parent = parent;
  100a65:	89 9e 88 00 00 00    	mov    DWORD PTR [esi+0x88],ebx
    dir->files = sll_new();
  100a6b:	89 86 80 00 00 00    	mov    DWORD PTR [esi+0x80],eax
        sll_add((u32)dir, parent->directories);
  100a71:	58                   	pop    eax
  100a72:	5a                   	pop    edx
  100a73:	ff b3 84 00 00 00    	push   DWORD PTR [ebx+0x84]
  100a79:	56                   	push   esi
  100a7a:	be 01 00 00 00       	mov    esi,0x1
  100a7f:	e8 cc 71 00 00       	call   107c50 <sll_add>
  100a84:	83 c4 10             	add    esp,0x10
  100a87:	e9 d4 fe ff ff       	jmp    100960 <directory_create+0x40>
  100a8c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]

00100a90 <directory_delete>:
{
  100a90:	83 ec 18             	sub    esp,0x18
    STUB(relative, path, recursive);
  100a93:	0f b6 44 24 24       	movzx  eax,BYTE PTR [esp+0x24]
  100a98:	50                   	push   eax
  100a99:	ff 74 24 24          	push   DWORD PTR [esp+0x24]
  100a9d:	ff 74 24 24          	push   DWORD PTR [esp+0x24]
  100aa1:	68 20 1d 11 00       	push   0x111d20
  100aa6:	68 a8 1e 11 00       	push   0x111ea8
  100aab:	e8 c0 5b 00 00       	call   106670 <printf>
}
  100ab0:	b8 01 00 00 00       	mov    eax,0x1
  100ab5:	83 c4 2c             	add    esp,0x2c
  100ab8:	c3                   	ret    
  100ab9:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]

00100ac0 <directory_existe>:
{
  100ac0:	55                   	push   ebp
  100ac1:	57                   	push   edi
  100ac2:	56                   	push   esi
  100ac3:	53                   	push   ebx
  100ac4:	81 ec 9c 00 00 00    	sub    esp,0x9c
  100aca:	8b 9c 24 b0 00 00 00 	mov    ebx,DWORD PTR [esp+0xb0]
  100ad1:	8d 7c 24 10          	lea    edi,[esp+0x10]
    directory_t * current = relative ? relative : root;
  100ad5:	85 db                	test   ebx,ebx
  100ad7:	0f 44 1d 00 ae 23 00 	cmove  ebx,DWORD PTR ds:0x23ae00
                    current = d;
  100ade:	31 f6                	xor    esi,esi
  100ae0:	eb 0f                	jmp    100af1 <directory_existe+0x31>
  100ae2:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
            current = current->parent;
  100ae8:	8b 9b 88 00 00 00    	mov    ebx,DWORD PTR [ebx+0x88]
    for (int i = 0; path_read(path, i, buffer); i++) 
  100aee:	83 c6 01             	add    esi,0x1
  100af1:	83 ec 04             	sub    esp,0x4
  100af4:	57                   	push   edi
  100af5:	56                   	push   esi
  100af6:	ff b4 24 c0 00 00 00 	push   DWORD PTR [esp+0xc0]
  100afd:	e8 1e 78 00 00       	call   108320 <path_read>
  100b02:	83 c4 10             	add    esp,0x10
  100b05:	85 c0                	test   eax,eax
  100b07:	74 67                	je     100b70 <directory_existe+0xb0>
        if (strcmp(buffer, "..") == 0)
  100b09:	83 ec 08             	sub    esp,0x8
  100b0c:	68 a0 21 11 00       	push   0x1121a0
  100b11:	57                   	push   edi
  100b12:	e8 69 50 00 00       	call   105b80 <strcmp>
  100b17:	83 c4 10             	add    esp,0x10
  100b1a:	85 c0                	test   eax,eax
  100b1c:	74 ca                	je     100ae8 <directory_existe+0x28>
        else if (strcmp(buffer, ".") == 0)
  100b1e:	83 ec 08             	sub    esp,0x8
  100b21:	68 eb 1e 11 00       	push   0x111eeb
  100b26:	57                   	push   edi
  100b27:	e8 54 50 00 00       	call   105b80 <strcmp>
  100b2c:	83 c4 10             	add    esp,0x10
  100b2f:	85 c0                	test   eax,eax
  100b31:	74 bb                	je     100aee <directory_existe+0x2e>
            SLL_FOREARCH(i, current->directories)
  100b33:	8b 83 84 00 00 00    	mov    eax,DWORD PTR [ebx+0x84]
  100b39:	8b 68 04             	mov    ebp,DWORD PTR [eax+0x4]
  100b3c:	85 ed                	test   ebp,ebp
  100b3e:	74 ae                	je     100aee <directory_existe+0x2e>
                directory_t *d = (directory_t *)i->data;
  100b40:	8b 4d 00             	mov    ecx,DWORD PTR [ebp+0x0]
                if (strcmp(buffer, d->name) == 0)
  100b43:	83 ec 08             	sub    esp,0x8
  100b46:	51                   	push   ecx
  100b47:	89 4c 24 18          	mov    DWORD PTR [esp+0x18],ecx
  100b4b:	57                   	push   edi
  100b4c:	e8 2f 50 00 00       	call   105b80 <strcmp>
  100b51:	83 c4 10             	add    esp,0x10
                    current = d;
  100b54:	8b 4c 24 0c          	mov    ecx,DWORD PTR [esp+0xc]
            SLL_FOREARCH(i, current->directories)
  100b58:	8b 6d 04             	mov    ebp,DWORD PTR [ebp+0x4]
                    current = d;
  100b5b:	85 c0                	test   eax,eax
  100b5d:	0f 44 d9             	cmove  ebx,ecx
            SLL_FOREARCH(i, current->directories)
  100b60:	85 ed                	test   ebp,ebp
  100b62:	75 dc                	jne    100b40 <directory_existe+0x80>
  100b64:	eb 88                	jmp    100aee <directory_existe+0x2e>
  100b66:	8d 76 00             	lea    esi,[esi+0x0]
  100b69:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
    if (filesystem_get_directory(relative, path) != NULL)
  100b70:	31 c0                	xor    eax,eax
  100b72:	85 db                	test   ebx,ebx
  100b74:	0f 95 c0             	setne  al
}
  100b77:	81 c4 9c 00 00 00    	add    esp,0x9c
  100b7d:	5b                   	pop    ebx
  100b7e:	5e                   	pop    esi
  100b7f:	5f                   	pop    edi
  100b80:	5d                   	pop    ebp
  100b81:	c3                   	ret    
  100b82:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
  100b89:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00100b90 <directory_copy>:
{
  100b90:	83 ec 14             	sub    esp,0x14
    STUB(relative_s, source, relative_d, destination);
  100b93:	ff 74 24 24          	push   DWORD PTR [esp+0x24]
  100b97:	ff 74 24 24          	push   DWORD PTR [esp+0x24]
  100b9b:	ff 74 24 24          	push   DWORD PTR [esp+0x24]
  100b9f:	ff 74 24 24          	push   DWORD PTR [esp+0x24]
  100ba3:	68 10 1d 11 00       	push   0x111d10
  100ba8:	68 a8 1e 11 00       	push   0x111ea8
  100bad:	e8 be 5a 00 00       	call   106670 <printf>
}
  100bb2:	b8 01 00 00 00       	mov    eax,0x1
  100bb7:	83 c4 2c             	add    esp,0x2c
  100bba:	c3                   	ret    
  100bbb:	90                   	nop
  100bbc:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]

00100bc0 <directory_move>:
{
  100bc0:	83 ec 14             	sub    esp,0x14
    STUB(relative_s, source, relative_d, destination);
  100bc3:	ff 74 24 24          	push   DWORD PTR [esp+0x24]
  100bc7:	ff 74 24 24          	push   DWORD PTR [esp+0x24]
  100bcb:	ff 74 24 24          	push   DWORD PTR [esp+0x24]
  100bcf:	ff 74 24 24          	push   DWORD PTR [esp+0x24]
  100bd3:	68 00 1d 11 00       	push   0x111d00
  100bd8:	68 a8 1e 11 00       	push   0x111ea8
  100bdd:	e8 8e 5a 00 00       	call   106670 <printf>
}
  100be2:	b8 01 00 00 00       	mov    eax,0x1
  100be7:	83 c4 2c             	add    esp,0x2c
  100bea:	c3                   	ret    
  100beb:	90                   	nop
  100bec:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]

00100bf0 <directory_open>:
{
  100bf0:	55                   	push   ebp
  100bf1:	57                   	push   edi
  100bf2:	56                   	push   esi
  100bf3:	53                   	push   ebx
  100bf4:	81 ec 9c 00 00 00    	sub    esp,0x9c
  100bfa:	8b 9c 24 b0 00 00 00 	mov    ebx,DWORD PTR [esp+0xb0]
  100c01:	8d 7c 24 10          	lea    edi,[esp+0x10]
    directory_t * current = relative ? relative : root;
  100c05:	85 db                	test   ebx,ebx
  100c07:	0f 44 1d 00 ae 23 00 	cmove  ebx,DWORD PTR ds:0x23ae00
                    current = d;
  100c0e:	31 f6                	xor    esi,esi
  100c10:	eb 0f                	jmp    100c21 <directory_open+0x31>
  100c12:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
            current = current->parent;
  100c18:	8b 9b 88 00 00 00    	mov    ebx,DWORD PTR [ebx+0x88]
    for (int i = 0; path_read(path, i, buffer); i++) 
  100c1e:	83 c6 01             	add    esi,0x1
  100c21:	83 ec 04             	sub    esp,0x4
  100c24:	57                   	push   edi
  100c25:	56                   	push   esi
  100c26:	ff b4 24 c0 00 00 00 	push   DWORD PTR [esp+0xc0]
  100c2d:	e8 ee 76 00 00       	call   108320 <path_read>
  100c32:	83 c4 10             	add    esp,0x10
  100c35:	85 c0                	test   eax,eax
  100c37:	74 67                	je     100ca0 <directory_open+0xb0>
        if (strcmp(buffer, "..") == 0)
  100c39:	83 ec 08             	sub    esp,0x8
  100c3c:	68 a0 21 11 00       	push   0x1121a0
  100c41:	57                   	push   edi
  100c42:	e8 39 4f 00 00       	call   105b80 <strcmp>
  100c47:	83 c4 10             	add    esp,0x10
  100c4a:	85 c0                	test   eax,eax
  100c4c:	74 ca                	je     100c18 <directory_open+0x28>
        else if (strcmp(buffer, ".") == 0)
  100c4e:	83 ec 08             	sub    esp,0x8
  100c51:	68 eb 1e 11 00       	push   0x111eeb
  100c56:	57                   	push   edi
  100c57:	e8 24 4f 00 00       	call   105b80 <strcmp>
  100c5c:	83 c4 10             	add    esp,0x10
  100c5f:	85 c0                	test   eax,eax
  100c61:	74 bb                	je     100c1e <directory_open+0x2e>
            SLL_FOREARCH(i, current->directories)
  100c63:	8b 83 84 00 00 00    	mov    eax,DWORD PTR [ebx+0x84]
  100c69:	8b 68 04             	mov    ebp,DWORD PTR [eax+0x4]
  100c6c:	85 ed                	test   ebp,ebp
  100c6e:	74 ae                	je     100c1e <directory_open+0x2e>
                directory_t *d = (directory_t *)i->data;
  100c70:	8b 4d 00             	mov    ecx,DWORD PTR [ebp+0x0]
                if (strcmp(buffer, d->name) == 0)
  100c73:	83 ec 08             	sub    esp,0x8
  100c76:	51                   	push   ecx
  100c77:	89 4c 24 18          	mov    DWORD PTR [esp+0x18],ecx
  100c7b:	57                   	push   edi
  100c7c:	e8 ff 4e 00 00       	call   105b80 <strcmp>
  100c81:	83 c4 10             	add    esp,0x10
                    current = d;
  100c84:	8b 4c 24 0c          	mov    ecx,DWORD PTR [esp+0xc]
            SLL_FOREARCH(i, current->directories)
  100c88:	8b 6d 04             	mov    ebp,DWORD PTR [ebp+0x4]
                    current = d;
  100c8b:	85 c0                	test   eax,eax
  100c8d:	0f 44 d9             	cmove  ebx,ecx
            SLL_FOREARCH(i, current->directories)
  100c90:	85 ed                	test   ebp,ebp
  100c92:	75 dc                	jne    100c70 <directory_open+0x80>
  100c94:	eb 88                	jmp    100c1e <directory_open+0x2e>
  100c96:	8d 76 00             	lea    esi,[esi+0x0]
  100c99:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
}
  100ca0:	81 c4 9c 00 00 00    	add    esp,0x9c
  100ca6:	89 d8                	mov    eax,ebx
  100ca8:	5b                   	pop    ebx
  100ca9:	5e                   	pop    esi
  100caa:	5f                   	pop    edi
  100cab:	5d                   	pop    ebp
  100cac:	c3                   	ret    
  100cad:	8d 76 00             	lea    esi,[esi+0x0]

00100cb0 <directory_close>:
}
  100cb0:	f3 c3                	repz ret 
  100cb2:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
  100cb9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00100cc0 <directory_get_files>:
{
  100cc0:	83 ec 0c             	sub    esp,0xc
  100cc3:	8b 4c 24 14          	mov    ecx,DWORD PTR [esp+0x14]
    SLL_FOREARCH(i, directory->files)
  100cc7:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
{
  100ccb:	8b 54 24 18          	mov    edx,DWORD PTR [esp+0x18]
    name[0] = '\0';
  100ccf:	c6 01 00             	mov    BYTE PTR [ecx],0x0
    SLL_FOREARCH(i, directory->files)
  100cd2:	8b 80 80 00 00 00    	mov    eax,DWORD PTR [eax+0x80]
  100cd8:	8b 40 04             	mov    eax,DWORD PTR [eax+0x4]
  100cdb:	85 c0                	test   eax,eax
  100cdd:	75 13                	jne    100cf2 <directory_get_files+0x32>
  100cdf:	eb 37                	jmp    100d18 <directory_get_files+0x58>
  100ce1:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
  100ce8:	8b 40 04             	mov    eax,DWORD PTR [eax+0x4]
        index--;
  100ceb:	83 ea 01             	sub    edx,0x1
    SLL_FOREARCH(i, directory->files)
  100cee:	85 c0                	test   eax,eax
  100cf0:	74 1b                	je     100d0d <directory_get_files+0x4d>
        if (index == 0)
  100cf2:	85 d2                	test   edx,edx
  100cf4:	75 f2                	jne    100ce8 <directory_get_files+0x28>
            strcpy(name, ((file_t *)i->data)->name);
  100cf6:	8b 00                	mov    eax,DWORD PTR [eax]
  100cf8:	83 ec 08             	sub    esp,0x8
  100cfb:	83 c0 0c             	add    eax,0xc
  100cfe:	50                   	push   eax
  100cff:	51                   	push   ecx
  100d00:	e8 0b 51 00 00       	call   105e10 <strcpy>
            return 1;
  100d05:	83 c4 10             	add    esp,0x10
  100d08:	b8 01 00 00 00       	mov    eax,0x1
}
  100d0d:	83 c4 0c             	add    esp,0xc
  100d10:	c3                   	ret    
  100d11:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
    return 0;
  100d18:	31 c0                	xor    eax,eax
  100d1a:	eb f1                	jmp    100d0d <directory_get_files+0x4d>
  100d1c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]

00100d20 <directory_get_directories>:
{
  100d20:	83 ec 0c             	sub    esp,0xc
  100d23:	8b 4c 24 14          	mov    ecx,DWORD PTR [esp+0x14]
    SLL_FOREARCH(i, directory->directories)
  100d27:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
{
  100d2b:	8b 54 24 18          	mov    edx,DWORD PTR [esp+0x18]
    name[0] = '\0';
  100d2f:	c6 01 00             	mov    BYTE PTR [ecx],0x0
    SLL_FOREARCH(i, directory->directories)
  100d32:	8b 80 84 00 00 00    	mov    eax,DWORD PTR [eax+0x84]
  100d38:	8b 40 04             	mov    eax,DWORD PTR [eax+0x4]
  100d3b:	85 c0                	test   eax,eax
  100d3d:	75 13                	jne    100d52 <directory_get_directories+0x32>
  100d3f:	eb 2f                	jmp    100d70 <directory_get_directories+0x50>
  100d41:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
  100d48:	8b 40 04             	mov    eax,DWORD PTR [eax+0x4]
        index--;
  100d4b:	83 ea 01             	sub    edx,0x1
    SLL_FOREARCH(i, directory->directories)
  100d4e:	85 c0                	test   eax,eax
  100d50:	74 17                	je     100d69 <directory_get_directories+0x49>
        if (index == 0)
  100d52:	85 d2                	test   edx,edx
  100d54:	75 f2                	jne    100d48 <directory_get_directories+0x28>
            strcpy(name, ((directory_t *)i->data)->name);
  100d56:	83 ec 08             	sub    esp,0x8
  100d59:	ff 30                	push   DWORD PTR [eax]
  100d5b:	51                   	push   ecx
  100d5c:	e8 af 50 00 00       	call   105e10 <strcpy>
            return 1;
  100d61:	83 c4 10             	add    esp,0x10
  100d64:	b8 01 00 00 00       	mov    eax,0x1
    }

    return 0;
}
  100d69:	83 c4 0c             	add    esp,0xc
  100d6c:	c3                   	ret    
  100d6d:	8d 76 00             	lea    esi,[esi+0x0]
    return 0;
  100d70:	31 c0                	xor    eax,eax
  100d72:	eb f5                	jmp    100d69 <directory_get_directories+0x49>
  100d74:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
  100d7a:	8d bf 00 00 00 00    	lea    edi,[edi+0x0]

00100d80 <alloc_file>:

/* === FILES ================================================================ */

file_t *alloc_file(const char *name)
{
  100d80:	53                   	push   ebx
  100d81:	83 ec 14             	sub    esp,0x14
    file_t *file = MALLOC(file_t);
  100d84:	68 90 00 00 00       	push   0x90
  100d89:	e8 12 62 00 00       	call   106fa0 <malloc>
  100d8e:	89 c3                	mov    ebx,eax

    strncpy((char *)&file->name, name, PATH_FILE_NAME_SIZE);
  100d90:	8d 40 0c             	lea    eax,[eax+0xc]
  100d93:	83 c4 0c             	add    esp,0xc
  100d96:	68 80 00 00 00       	push   0x80
  100d9b:	ff 74 24 18          	push   DWORD PTR [esp+0x18]
  100d9f:	50                   	push   eax
  100da0:	e8 9b 50 00 00       	call   105e40 <strncpy>

    return file;
}
  100da5:	83 c4 18             	add    esp,0x18
  100da8:	89 d8                	mov    eax,ebx
  100daa:	5b                   	pop    ebx
  100dab:	c3                   	ret    
  100dac:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]

00100db0 <file_create>:

/* --- Create/Delete/Existe ------------------------------------------------- */

int file_create(directory_t *relative, const char *path, filesystem_t * fs, int device, int inode)
{
  100db0:	55                   	push   ebp
  100db1:	57                   	push   edi
  100db2:	56                   	push   esi
  100db3:	53                   	push   ebx
  100db4:	81 ec 28 01 00 00    	sub    esp,0x128
  100dba:	8b ac 24 40 01 00 00 	mov    ebp,DWORD PTR [esp+0x140]
  100dc1:	8b 9c 24 3c 01 00 00 	mov    ebx,DWORD PTR [esp+0x13c]
    char *dir_path = malloc(strlen(path));
  100dc8:	55                   	push   ebp
  100dc9:	e8 f2 52 00 00       	call   1060c0 <strlen>
  100dce:	89 04 24             	mov    DWORD PTR [esp],eax
  100dd1:	e8 ca 61 00 00       	call   106fa0 <malloc>
  100dd6:	89 44 24 18          	mov    DWORD PTR [esp+0x18],eax
    char file_name[128];
    file_t *file = NULL;

    if (path_split(path, dir_path, file_name))
  100dda:	83 c4 0c             	add    esp,0xc
  100ddd:	8d 4c 24 14          	lea    ecx,[esp+0x14]
  100de1:	51                   	push   ecx
  100de2:	50                   	push   eax
  100de3:	55                   	push   ebp
  100de4:	e8 a7 76 00 00       	call   108490 <path_split>
  100de9:	83 c4 10             	add    esp,0x10
  100dec:	85 c0                	test   eax,eax
  100dee:	89 c6                	mov    esi,eax
  100df0:	75 1e                	jne    100e10 <file_create+0x60>
        file->fs = fs;
        file->device = device;
        file->inode = inode;
    }

    free(dir_path);
  100df2:	83 ec 0c             	sub    esp,0xc
  100df5:	ff 74 24 14          	push   DWORD PTR [esp+0x14]
  100df9:	e8 72 66 00 00       	call   107470 <free>
    return file == NULL ? 0 : 1;
}
  100dfe:	81 c4 2c 01 00 00    	add    esp,0x12c
  100e04:	89 f0                	mov    eax,esi
  100e06:	5b                   	pop    ebx
  100e07:	5e                   	pop    esi
  100e08:	5f                   	pop    edi
  100e09:	5d                   	pop    ebp
  100e0a:	c3                   	ret    
  100e0b:	90                   	nop
  100e0c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
    directory_t * current = relative ? relative : root;
  100e10:	85 db                	test   ebx,ebx
  100e12:	8d bc 24 90 00 00 00 	lea    edi,[esp+0x90]
  100e19:	0f 44 1d 00 ae 23 00 	cmove  ebx,DWORD PTR ds:0x23ae00
                    current = d;
  100e20:	31 f6                	xor    esi,esi
  100e22:	eb 0d                	jmp    100e31 <file_create+0x81>
  100e24:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
            current = current->parent;
  100e28:	8b 9b 88 00 00 00    	mov    ebx,DWORD PTR [ebx+0x88]
    for (int i = 0; path_read(path, i, buffer); i++) 
  100e2e:	83 c6 01             	add    esi,0x1
  100e31:	83 ec 04             	sub    esp,0x4
  100e34:	57                   	push   edi
  100e35:	56                   	push   esi
  100e36:	55                   	push   ebp
  100e37:	e8 e4 74 00 00       	call   108320 <path_read>
  100e3c:	83 c4 10             	add    esp,0x10
  100e3f:	85 c0                	test   eax,eax
  100e41:	74 7d                	je     100ec0 <file_create+0x110>
        if (strcmp(buffer, "..") == 0)
  100e43:	83 ec 08             	sub    esp,0x8
  100e46:	68 a0 21 11 00       	push   0x1121a0
  100e4b:	57                   	push   edi
  100e4c:	e8 2f 4d 00 00       	call   105b80 <strcmp>
  100e51:	83 c4 10             	add    esp,0x10
  100e54:	85 c0                	test   eax,eax
  100e56:	74 d0                	je     100e28 <file_create+0x78>
        else if (strcmp(buffer, ".") == 0)
  100e58:	83 ec 08             	sub    esp,0x8
  100e5b:	68 eb 1e 11 00       	push   0x111eeb
  100e60:	57                   	push   edi
  100e61:	e8 1a 4d 00 00       	call   105b80 <strcmp>
  100e66:	83 c4 10             	add    esp,0x10
  100e69:	85 c0                	test   eax,eax
  100e6b:	74 c1                	je     100e2e <file_create+0x7e>
            SLL_FOREARCH(i, current->directories)
  100e6d:	8b 83 84 00 00 00    	mov    eax,DWORD PTR [ebx+0x84]
  100e73:	8b 50 04             	mov    edx,DWORD PTR [eax+0x4]
  100e76:	85 d2                	test   edx,edx
  100e78:	74 b4                	je     100e2e <file_create+0x7e>
  100e7a:	89 74 24 0c          	mov    DWORD PTR [esp+0xc],esi
  100e7e:	89 ac 24 34 01 00 00 	mov    DWORD PTR [esp+0x134],ebp
  100e85:	89 de                	mov    esi,ebx
  100e87:	89 d3                	mov    ebx,edx
  100e89:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
                directory_t *d = (directory_t *)i->data;
  100e90:	8b 2b                	mov    ebp,DWORD PTR [ebx]
                if (strcmp(buffer, d->name) == 0)
  100e92:	83 ec 08             	sub    esp,0x8
  100e95:	55                   	push   ebp
  100e96:	57                   	push   edi
  100e97:	e8 e4 4c 00 00       	call   105b80 <strcmp>
            SLL_FOREARCH(i, current->directories)
  100e9c:	8b 5b 04             	mov    ebx,DWORD PTR [ebx+0x4]
                if (strcmp(buffer, d->name) == 0)
  100e9f:	83 c4 10             	add    esp,0x10
                    current = d;
  100ea2:	85 c0                	test   eax,eax
  100ea4:	0f 44 f5             	cmove  esi,ebp
            SLL_FOREARCH(i, current->directories)
  100ea7:	85 db                	test   ebx,ebx
  100ea9:	75 e5                	jne    100e90 <file_create+0xe0>
  100eab:	89 f3                	mov    ebx,esi
  100ead:	8b ac 24 34 01 00 00 	mov    ebp,DWORD PTR [esp+0x134]
  100eb4:	8b 74 24 0c          	mov    esi,DWORD PTR [esp+0xc]
  100eb8:	e9 71 ff ff ff       	jmp    100e2e <file_create+0x7e>
  100ebd:	8d 76 00             	lea    esi,[esi+0x0]
    file_t *file = MALLOC(file_t);
  100ec0:	83 ec 0c             	sub    esp,0xc
  100ec3:	68 90 00 00 00       	push   0x90
  100ec8:	e8 d3 60 00 00       	call   106fa0 <malloc>
    strncpy((char *)&file->name, name, PATH_FILE_NAME_SIZE);
  100ecd:	83 c4 0c             	add    esp,0xc
    file_t *file = MALLOC(file_t);
  100ed0:	89 c6                	mov    esi,eax
    strncpy((char *)&file->name, name, PATH_FILE_NAME_SIZE);
  100ed2:	68 80 00 00 00       	push   0x80
  100ed7:	8d 44 24 18          	lea    eax,[esp+0x18]
  100edb:	50                   	push   eax
  100edc:	8d 46 0c             	lea    eax,[esi+0xc]
  100edf:	50                   	push   eax
  100ee0:	e8 5b 4f 00 00       	call   105e40 <strncpy>
        file->parent = parent;
  100ee5:	89 9e 8c 00 00 00    	mov    DWORD PTR [esi+0x8c],ebx
        sll_add((u32)file, parent->files);
  100eeb:	58                   	pop    eax
  100eec:	5a                   	pop    edx
  100eed:	ff b3 80 00 00 00    	push   DWORD PTR [ebx+0x80]
  100ef3:	56                   	push   esi
  100ef4:	e8 57 6d 00 00       	call   107c50 <sll_add>
        file->fs = fs;
  100ef9:	8b 84 24 48 01 00 00 	mov    eax,DWORD PTR [esp+0x148]
  100f00:	89 46 08             	mov    DWORD PTR [esi+0x8],eax
        file->device = device;
  100f03:	8b 84 24 4c 01 00 00 	mov    eax,DWORD PTR [esp+0x14c]
  100f0a:	89 06                	mov    DWORD PTR [esi],eax
        file->inode = inode;
  100f0c:	8b 84 24 50 01 00 00 	mov    eax,DWORD PTR [esp+0x150]
  100f13:	83 c4 10             	add    esp,0x10
  100f16:	89 46 04             	mov    DWORD PTR [esi+0x4],eax
  100f19:	be 01 00 00 00       	mov    esi,0x1
  100f1e:	e9 cf fe ff ff       	jmp    100df2 <file_create+0x42>
  100f23:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
  100f29:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00100f30 <file_delete>:

int file_delete(directory_t *relative, const char *path)
{
  100f30:	83 ec 0c             	sub    esp,0xc
    STUB(relative, path);
  100f33:	ff 74 24 14          	push   DWORD PTR [esp+0x14]
  100f37:	ff 74 24 14          	push   DWORD PTR [esp+0x14]
  100f3b:	68 f4 1c 11 00       	push   0x111cf4
  100f40:	68 a8 1e 11 00       	push   0x111ea8
  100f45:	e8 26 57 00 00       	call   106670 <printf>
    return 1;
}
  100f4a:	b8 01 00 00 00       	mov    eax,0x1
  100f4f:	83 c4 1c             	add    esp,0x1c
  100f52:	c3                   	ret    
  100f53:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
  100f59:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00100f60 <file_existe>:

int file_existe(directory_t *relative, const char *path)
{
  100f60:	55                   	push   ebp
  100f61:	57                   	push   edi
  100f62:	56                   	push   esi
  100f63:	53                   	push   ebx
  100f64:	81 ec 28 01 00 00    	sub    esp,0x128
  100f6a:	8b b4 24 40 01 00 00 	mov    esi,DWORD PTR [esp+0x140]
  100f71:	8b 9c 24 3c 01 00 00 	mov    ebx,DWORD PTR [esp+0x13c]
    char *dir_name = malloc(strlen(path));
  100f78:	56                   	push   esi
  100f79:	e8 42 51 00 00       	call   1060c0 <strlen>
  100f7e:	89 04 24             	mov    DWORD PTR [esp],eax
  100f81:	e8 1a 60 00 00       	call   106fa0 <malloc>
    if (path_split(path, dir_name, file_name))
  100f86:	83 c4 0c             	add    esp,0xc
    char *dir_name = malloc(strlen(path));
  100f89:	89 c5                	mov    ebp,eax
    if (path_split(path, dir_name, file_name))
  100f8b:	8d 44 24 14          	lea    eax,[esp+0x14]
  100f8f:	50                   	push   eax
  100f90:	55                   	push   ebp
  100f91:	56                   	push   esi
  100f92:	e8 f9 74 00 00       	call   108490 <path_split>
  100f97:	83 c4 10             	add    esp,0x10
  100f9a:	85 c0                	test   eax,eax
  100f9c:	0f 84 e7 00 00 00    	je     101089 <file_existe+0x129>
    directory_t * current = relative ? relative : root;
  100fa2:	85 db                	test   ebx,ebx
  100fa4:	8d bc 24 90 00 00 00 	lea    edi,[esp+0x90]
  100fab:	0f 44 1d 00 ae 23 00 	cmove  ebx,DWORD PTR ds:0x23ae00
                    current = d;
  100fb2:	31 f6                	xor    esi,esi
  100fb4:	eb 13                	jmp    100fc9 <file_existe+0x69>
  100fb6:	8d 76 00             	lea    esi,[esi+0x0]
  100fb9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
            current = current->parent;
  100fc0:	8b 9b 88 00 00 00    	mov    ebx,DWORD PTR [ebx+0x88]
    for (int i = 0; path_read(path, i, buffer); i++) 
  100fc6:	83 c6 01             	add    esi,0x1
  100fc9:	83 ec 04             	sub    esp,0x4
  100fcc:	57                   	push   edi
  100fcd:	56                   	push   esi
  100fce:	55                   	push   ebp
  100fcf:	e8 4c 73 00 00       	call   108320 <path_read>
  100fd4:	83 c4 10             	add    esp,0x10
  100fd7:	85 c0                	test   eax,eax
  100fd9:	74 75                	je     101050 <file_existe+0xf0>
        if (strcmp(buffer, "..") == 0)
  100fdb:	83 ec 08             	sub    esp,0x8
  100fde:	68 a0 21 11 00       	push   0x1121a0
  100fe3:	57                   	push   edi
  100fe4:	e8 97 4b 00 00       	call   105b80 <strcmp>
  100fe9:	83 c4 10             	add    esp,0x10
  100fec:	85 c0                	test   eax,eax
  100fee:	74 d0                	je     100fc0 <file_existe+0x60>
        else if (strcmp(buffer, ".") == 0)
  100ff0:	83 ec 08             	sub    esp,0x8
  100ff3:	68 eb 1e 11 00       	push   0x111eeb
  100ff8:	57                   	push   edi
  100ff9:	e8 82 4b 00 00       	call   105b80 <strcmp>
  100ffe:	83 c4 10             	add    esp,0x10
  101001:	85 c0                	test   eax,eax
  101003:	74 c1                	je     100fc6 <file_existe+0x66>
            SLL_FOREARCH(i, current->directories)
  101005:	8b 83 84 00 00 00    	mov    eax,DWORD PTR [ebx+0x84]
  10100b:	8b 50 04             	mov    edx,DWORD PTR [eax+0x4]
  10100e:	85 d2                	test   edx,edx
  101010:	74 b4                	je     100fc6 <file_existe+0x66>
  101012:	89 74 24 0c          	mov    DWORD PTR [esp+0xc],esi
  101016:	89 6c 24 08          	mov    DWORD PTR [esp+0x8],ebp
  10101a:	89 de                	mov    esi,ebx
  10101c:	89 d3                	mov    ebx,edx
  10101e:	66 90                	xchg   ax,ax
                directory_t *d = (directory_t *)i->data;
  101020:	8b 2b                	mov    ebp,DWORD PTR [ebx]
                if (strcmp(buffer, d->name) == 0)
  101022:	83 ec 08             	sub    esp,0x8
  101025:	55                   	push   ebp
  101026:	57                   	push   edi
  101027:	e8 54 4b 00 00       	call   105b80 <strcmp>
            SLL_FOREARCH(i, current->directories)
  10102c:	8b 5b 04             	mov    ebx,DWORD PTR [ebx+0x4]
                if (strcmp(buffer, d->name) == 0)
  10102f:	83 c4 10             	add    esp,0x10
                    current = d;
  101032:	85 c0                	test   eax,eax
  101034:	0f 44 f5             	cmove  esi,ebp
            SLL_FOREARCH(i, current->directories)
  101037:	85 db                	test   ebx,ebx
  101039:	75 e5                	jne    101020 <file_existe+0xc0>
  10103b:	89 f3                	mov    ebx,esi
  10103d:	8b 6c 24 08          	mov    ebp,DWORD PTR [esp+0x8]
  101041:	8b 74 24 0c          	mov    esi,DWORD PTR [esp+0xc]
  101045:	e9 7c ff ff ff       	jmp    100fc6 <file_existe+0x66>
  10104a:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
        SLL_FOREARCH(i, dir->files)
  101050:	8b 93 80 00 00 00    	mov    edx,DWORD PTR [ebx+0x80]
  101056:	8b 5a 04             	mov    ebx,DWORD PTR [edx+0x4]
  101059:	85 db                	test   ebx,ebx
  10105b:	74 2c                	je     101089 <file_existe+0x129>
    file_t *file = NULL;
  10105d:	31 ff                	xor    edi,edi
  10105f:	90                   	nop
            file_t* f = (file_t *)i->data;
  101060:	8b 33                	mov    esi,DWORD PTR [ebx]
            if (strcmp(file_name, f->name) == 0)
  101062:	83 ec 08             	sub    esp,0x8
  101065:	8d 46 0c             	lea    eax,[esi+0xc]
  101068:	50                   	push   eax
  101069:	8d 44 24 1c          	lea    eax,[esp+0x1c]
  10106d:	50                   	push   eax
  10106e:	e8 0d 4b 00 00       	call   105b80 <strcmp>
        SLL_FOREARCH(i, dir->files)
  101073:	8b 5b 04             	mov    ebx,DWORD PTR [ebx+0x4]
            if (strcmp(file_name, f->name) == 0)
  101076:	83 c4 10             	add    esp,0x10
                file = f;
  101079:	85 c0                	test   eax,eax
  10107b:	0f 44 fe             	cmove  edi,esi
        SLL_FOREARCH(i, dir->files)
  10107e:	85 db                	test   ebx,ebx
  101080:	75 de                	jne    101060 <file_existe+0x100>
  101082:	31 c0                	xor    eax,eax
  101084:	85 ff                	test   edi,edi
  101086:	0f 95 c0             	setne  al
  101089:	89 44 24 08          	mov    DWORD PTR [esp+0x8],eax
    free(dir_name);
  10108d:	83 ec 0c             	sub    esp,0xc
  101090:	55                   	push   ebp
  101091:	e8 da 63 00 00       	call   107470 <free>
  101096:	83 c4 10             	add    esp,0x10
  101099:	8b 44 24 08          	mov    eax,DWORD PTR [esp+0x8]
    }
    else
    {
        return 0;
    }
}
  10109d:	81 c4 1c 01 00 00    	add    esp,0x11c
  1010a3:	5b                   	pop    ebx
  1010a4:	5e                   	pop    esi
  1010a5:	5f                   	pop    edi
  1010a6:	5d                   	pop    ebp
  1010a7:	c3                   	ret    
  1010a8:	90                   	nop
  1010a9:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]

001010b0 <file_copy>:

/* --- Move/Copy ------------------------------------------------------------ */

int file_copy(directory_t *relative_s, const char *source, directory_t *relative_d, const char *destination)
{
  1010b0:	83 ec 14             	sub    esp,0x14
    STUB(relative_s, source, relative_d, destination);
  1010b3:	ff 74 24 24          	push   DWORD PTR [esp+0x24]
  1010b7:	ff 74 24 24          	push   DWORD PTR [esp+0x24]
  1010bb:	ff 74 24 24          	push   DWORD PTR [esp+0x24]
  1010bf:	ff 74 24 24          	push   DWORD PTR [esp+0x24]
  1010c3:	68 e8 1c 11 00       	push   0x111ce8
  1010c8:	68 a8 1e 11 00       	push   0x111ea8
  1010cd:	e8 9e 55 00 00       	call   106670 <printf>
    return 1;
}
  1010d2:	b8 01 00 00 00       	mov    eax,0x1
  1010d7:	83 c4 2c             	add    esp,0x2c
  1010da:	c3                   	ret    
  1010db:	90                   	nop
  1010dc:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]

001010e0 <file_move>:

int file_move(directory_t *relative_s, const char *source, directory_t *relative_d, const char *destination)
{
  1010e0:	83 ec 14             	sub    esp,0x14
    STUB(relative_s, source, relative_d, destination);
  1010e3:	ff 74 24 24          	push   DWORD PTR [esp+0x24]
  1010e7:	ff 74 24 24          	push   DWORD PTR [esp+0x24]
  1010eb:	ff 74 24 24          	push   DWORD PTR [esp+0x24]
  1010ef:	ff 74 24 24          	push   DWORD PTR [esp+0x24]
  1010f3:	68 dc 1c 11 00       	push   0x111cdc
  1010f8:	68 a8 1e 11 00       	push   0x111ea8
  1010fd:	e8 6e 55 00 00       	call   106670 <printf>
    return 1;
}
  101102:	b8 01 00 00 00       	mov    eax,0x1
  101107:	83 c4 2c             	add    esp,0x2c
  10110a:	c3                   	ret    
  10110b:	90                   	nop
  10110c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]

00101110 <file_open>:

/* --- Open/Close/Read/Write ------------------------------------------------ */

file_t *file_open(directory_t *relative, const char *path)
{
  101110:	55                   	push   ebp
  101111:	57                   	push   edi
  101112:	56                   	push   esi
  101113:	53                   	push   ebx
  101114:	81 ec 28 01 00 00    	sub    esp,0x128
  10111a:	8b b4 24 40 01 00 00 	mov    esi,DWORD PTR [esp+0x140]
  101121:	8b 9c 24 3c 01 00 00 	mov    ebx,DWORD PTR [esp+0x13c]
    char *dir_name = malloc(strlen(path));
  101128:	56                   	push   esi
  101129:	e8 92 4f 00 00       	call   1060c0 <strlen>
  10112e:	89 04 24             	mov    DWORD PTR [esp],eax
  101131:	e8 6a 5e 00 00       	call   106fa0 <malloc>
    if (path_split(path, dir_name, file_name))
  101136:	83 c4 0c             	add    esp,0xc
    char *dir_name = malloc(strlen(path));
  101139:	89 c5                	mov    ebp,eax
    if (path_split(path, dir_name, file_name))
  10113b:	8d 44 24 14          	lea    eax,[esp+0x14]
  10113f:	50                   	push   eax
  101140:	55                   	push   ebp
  101141:	56                   	push   esi
  101142:	e8 49 73 00 00       	call   108490 <path_split>
  101147:	83 c4 10             	add    esp,0x10
  10114a:	85 c0                	test   eax,eax
  10114c:	0f 84 16 01 00 00    	je     101268 <file_open+0x158>
    directory_t * current = relative ? relative : root;
  101152:	85 db                	test   ebx,ebx
  101154:	8d bc 24 90 00 00 00 	lea    edi,[esp+0x90]
  10115b:	0f 44 1d 00 ae 23 00 	cmove  ebx,DWORD PTR ds:0x23ae00
                    current = d;
  101162:	31 f6                	xor    esi,esi
  101164:	eb 13                	jmp    101179 <file_open+0x69>
  101166:	8d 76 00             	lea    esi,[esi+0x0]
  101169:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
            current = current->parent;
  101170:	8b 9b 88 00 00 00    	mov    ebx,DWORD PTR [ebx+0x88]
    for (int i = 0; path_read(path, i, buffer); i++) 
  101176:	83 c6 01             	add    esi,0x1
  101179:	83 ec 04             	sub    esp,0x4
  10117c:	57                   	push   edi
  10117d:	56                   	push   esi
  10117e:	55                   	push   ebp
  10117f:	e8 9c 71 00 00       	call   108320 <path_read>
  101184:	83 c4 10             	add    esp,0x10
  101187:	85 c0                	test   eax,eax
  101189:	74 75                	je     101200 <file_open+0xf0>
        if (strcmp(buffer, "..") == 0)
  10118b:	83 ec 08             	sub    esp,0x8
  10118e:	68 a0 21 11 00       	push   0x1121a0
  101193:	57                   	push   edi
  101194:	e8 e7 49 00 00       	call   105b80 <strcmp>
  101199:	83 c4 10             	add    esp,0x10
  10119c:	85 c0                	test   eax,eax
  10119e:	74 d0                	je     101170 <file_open+0x60>
        else if (strcmp(buffer, ".") == 0)
  1011a0:	83 ec 08             	sub    esp,0x8
  1011a3:	68 eb 1e 11 00       	push   0x111eeb
  1011a8:	57                   	push   edi
  1011a9:	e8 d2 49 00 00       	call   105b80 <strcmp>
  1011ae:	83 c4 10             	add    esp,0x10
  1011b1:	85 c0                	test   eax,eax
  1011b3:	74 c1                	je     101176 <file_open+0x66>
            SLL_FOREARCH(i, current->directories)
  1011b5:	8b 83 84 00 00 00    	mov    eax,DWORD PTR [ebx+0x84]
  1011bb:	8b 50 04             	mov    edx,DWORD PTR [eax+0x4]
  1011be:	85 d2                	test   edx,edx
  1011c0:	74 b4                	je     101176 <file_open+0x66>
  1011c2:	89 74 24 0c          	mov    DWORD PTR [esp+0xc],esi
  1011c6:	89 6c 24 08          	mov    DWORD PTR [esp+0x8],ebp
  1011ca:	89 de                	mov    esi,ebx
  1011cc:	89 d3                	mov    ebx,edx
  1011ce:	66 90                	xchg   ax,ax
                directory_t *d = (directory_t *)i->data;
  1011d0:	8b 2b                	mov    ebp,DWORD PTR [ebx]
                if (strcmp(buffer, d->name) == 0)
  1011d2:	83 ec 08             	sub    esp,0x8
  1011d5:	55                   	push   ebp
  1011d6:	57                   	push   edi
  1011d7:	e8 a4 49 00 00       	call   105b80 <strcmp>
            SLL_FOREARCH(i, current->directories)
  1011dc:	8b 5b 04             	mov    ebx,DWORD PTR [ebx+0x4]
                if (strcmp(buffer, d->name) == 0)
  1011df:	83 c4 10             	add    esp,0x10
                    current = d;
  1011e2:	85 c0                	test   eax,eax
  1011e4:	0f 44 f5             	cmove  esi,ebp
            SLL_FOREARCH(i, current->directories)
  1011e7:	85 db                	test   ebx,ebx
  1011e9:	75 e5                	jne    1011d0 <file_open+0xc0>
  1011eb:	89 f3                	mov    ebx,esi
  1011ed:	8b 6c 24 08          	mov    ebp,DWORD PTR [esp+0x8]
  1011f1:	8b 74 24 0c          	mov    esi,DWORD PTR [esp+0xc]
  1011f5:	e9 7c ff ff ff       	jmp    101176 <file_open+0x66>
  1011fa:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
        SLL_FOREARCH(i, dir->files)
  101200:	8b 83 80 00 00 00    	mov    eax,DWORD PTR [ebx+0x80]
  101206:	8b 58 04             	mov    ebx,DWORD PTR [eax+0x4]
  101209:	85 db                	test   ebx,ebx
  10120b:	74 5b                	je     101268 <file_open+0x158>
    file_t *file = NULL;
  10120d:	31 ff                	xor    edi,edi
  10120f:	90                   	nop
            file_t* f = (file_t *)i->data;
  101210:	8b 33                	mov    esi,DWORD PTR [ebx]
            if (strcmp(file_name, f->name) == 0)
  101212:	83 ec 08             	sub    esp,0x8
  101215:	8d 46 0c             	lea    eax,[esi+0xc]
  101218:	50                   	push   eax
  101219:	8d 44 24 1c          	lea    eax,[esp+0x1c]
  10121d:	50                   	push   eax
  10121e:	e8 5d 49 00 00       	call   105b80 <strcmp>
        SLL_FOREARCH(i, dir->files)
  101223:	8b 5b 04             	mov    ebx,DWORD PTR [ebx+0x4]
            if (strcmp(file_name, f->name) == 0)
  101226:	83 c4 10             	add    esp,0x10
                file = f;
  101229:	85 c0                	test   eax,eax
  10122b:	0f 44 fe             	cmove  edi,esi
        SLL_FOREARCH(i, dir->files)
  10122e:	85 db                	test   ebx,ebx
  101230:	75 de                	jne    101210 <file_open+0x100>
    free(dir_name);
  101232:	83 ec 0c             	sub    esp,0xc
  101235:	55                   	push   ebp
  101236:	e8 35 62 00 00       	call   107470 <free>
    file_t * file = filesystem_get_file(relative, path);

    if (file != NULL && (file->fs->file_open == NULL || file->fs->file_open(file)))
  10123b:	83 c4 10             	add    esp,0x10
  10123e:	85 ff                	test   edi,edi
  101240:	74 46                	je     101288 <file_open+0x178>
  101242:	8b 47 08             	mov    eax,DWORD PTR [edi+0x8]
  101245:	8b 00                	mov    eax,DWORD PTR [eax]
  101247:	85 c0                	test   eax,eax
  101249:	74 0d                	je     101258 <file_open+0x148>
  10124b:	83 ec 0c             	sub    esp,0xc
  10124e:	57                   	push   edi
  10124f:	ff d0                	call   eax
  101251:	83 c4 10             	add    esp,0x10
  101254:	85 c0                	test   eax,eax
  101256:	74 30                	je     101288 <file_open+0x178>
    }
    else
    {
        return NULL;
    }
}
  101258:	81 c4 1c 01 00 00    	add    esp,0x11c
  10125e:	89 f8                	mov    eax,edi
  101260:	5b                   	pop    ebx
  101261:	5e                   	pop    esi
  101262:	5f                   	pop    edi
  101263:	5d                   	pop    ebp
  101264:	c3                   	ret    
  101265:	8d 76 00             	lea    esi,[esi+0x0]
    free(dir_name);
  101268:	83 ec 0c             	sub    esp,0xc
        return NULL;
  10126b:	31 ff                	xor    edi,edi
    free(dir_name);
  10126d:	55                   	push   ebp
  10126e:	e8 fd 61 00 00       	call   107470 <free>
  101273:	83 c4 10             	add    esp,0x10
}
  101276:	89 f8                	mov    eax,edi
  101278:	81 c4 1c 01 00 00    	add    esp,0x11c
  10127e:	5b                   	pop    ebx
  10127f:	5e                   	pop    esi
  101280:	5f                   	pop    edi
  101281:	5d                   	pop    ebp
  101282:	c3                   	ret    
  101283:	90                   	nop
  101284:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
        return NULL;
  101288:	31 ff                	xor    edi,edi
  10128a:	eb cc                	jmp    101258 <file_open+0x148>
  10128c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]

00101290 <file_close>:

void file_close(file_t *file)
{
  101290:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
    if (file != NULL && file->fs->file_close != NULL)
  101294:	85 c0                	test   eax,eax
  101296:	74 10                	je     1012a8 <file_close+0x18>
  101298:	8b 40 08             	mov    eax,DWORD PTR [eax+0x8]
  10129b:	8b 40 04             	mov    eax,DWORD PTR [eax+0x4]
  10129e:	85 c0                	test   eax,eax
  1012a0:	74 06                	je     1012a8 <file_close+0x18>
    {
        file->fs->file_close(file);
  1012a2:	ff e0                	jmp    eax
  1012a4:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
    }
}
  1012a8:	f3 c3                	repz ret 
  1012aa:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]

001012b0 <file_stat>:

void file_stat(file_t * file, fstat_t * stat)
{
  1012b0:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
    if (file != NULL && file->fs->file_stat != NULL)
  1012b4:	85 c0                	test   eax,eax
  1012b6:	74 10                	je     1012c8 <file_stat+0x18>
  1012b8:	8b 40 08             	mov    eax,DWORD PTR [eax+0x8]
  1012bb:	8b 40 10             	mov    eax,DWORD PTR [eax+0x10]
  1012be:	85 c0                	test   eax,eax
  1012c0:	74 06                	je     1012c8 <file_stat+0x18>
    {
        file->fs->file_stat(file, stat);
  1012c2:	ff e0                	jmp    eax
  1012c4:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
    }
}
  1012c8:	f3 c3                	repz ret 
  1012ca:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]

001012d0 <file_read>:

int file_read(file_t *file, uint offset, void *buffer, uint n)
{
  1012d0:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
    if (file != NULL && file->fs->file_read !=NULL)
  1012d4:	85 c0                	test   eax,eax
  1012d6:	74 10                	je     1012e8 <file_read+0x18>
  1012d8:	8b 40 08             	mov    eax,DWORD PTR [eax+0x8]
  1012db:	8b 40 08             	mov    eax,DWORD PTR [eax+0x8]
  1012de:	85 c0                	test   eax,eax
  1012e0:	74 06                	je     1012e8 <file_read+0x18>
    {
        return file->fs->file_read(file, offset, buffer, n);
  1012e2:	ff e0                	jmp    eax
  1012e4:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
    }

    return 0;
}
  1012e8:	31 c0                	xor    eax,eax
  1012ea:	c3                   	ret    
  1012eb:	90                   	nop
  1012ec:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]

001012f0 <file_read_all>:

void* file_read_all(file_t* file)
{
  1012f0:	56                   	push   esi
  1012f1:	53                   	push   ebx
  1012f2:	83 ec 14             	sub    esp,0x14
  1012f5:	8b 5c 24 20          	mov    ebx,DWORD PTR [esp+0x20]
    void* data = NULL;

    if (file != NULL)
  1012f9:	85 db                	test   ebx,ebx
  1012fb:	74 53                	je     101350 <file_read_all+0x60>
    if (file != NULL && file->fs->file_stat != NULL)
  1012fd:	8b 43 08             	mov    eax,DWORD PTR [ebx+0x8]
  101300:	8b 40 10             	mov    eax,DWORD PTR [eax+0x10]
  101303:	85 c0                	test   eax,eax
  101305:	74 0e                	je     101315 <file_read_all+0x25>
        file->fs->file_stat(file, stat);
  101307:	83 ec 08             	sub    esp,0x8
  10130a:	8d 54 24 0c          	lea    edx,[esp+0xc]
  10130e:	52                   	push   edx
  10130f:	53                   	push   ebx
  101310:	ff d0                	call   eax
  101312:	83 c4 10             	add    esp,0x10
    {
        fstat_t stat;
        file_stat(file, &stat);

        data = malloc(stat.size);
  101315:	83 ec 0c             	sub    esp,0xc
  101318:	ff 74 24 10          	push   DWORD PTR [esp+0x10]
  10131c:	e8 7f 5c 00 00       	call   106fa0 <malloc>
  101321:	89 c6                	mov    esi,eax
    if (file != NULL && file->fs->file_read !=NULL)
  101323:	8b 43 08             	mov    eax,DWORD PTR [ebx+0x8]
        file_read(file, 0, data, stat.size);
  101326:	8b 54 24 14          	mov    edx,DWORD PTR [esp+0x14]
    if (file != NULL && file->fs->file_read !=NULL)
  10132a:	83 c4 10             	add    esp,0x10
  10132d:	8b 40 08             	mov    eax,DWORD PTR [eax+0x8]
  101330:	85 c0                	test   eax,eax
  101332:	74 0a                	je     10133e <file_read_all+0x4e>
        return file->fs->file_read(file, offset, buffer, n);
  101334:	52                   	push   edx
  101335:	56                   	push   esi
  101336:	6a 00                	push   0x0
  101338:	53                   	push   ebx
  101339:	ff d0                	call   eax
  10133b:	83 c4 10             	add    esp,0x10
    }

    return data;
}
  10133e:	83 c4 14             	add    esp,0x14
  101341:	89 f0                	mov    eax,esi
  101343:	5b                   	pop    ebx
  101344:	5e                   	pop    esi
  101345:	c3                   	ret    
  101346:	8d 76 00             	lea    esi,[esi+0x0]
  101349:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
    void* data = NULL;
  101350:	31 f6                	xor    esi,esi
    return data;
  101352:	eb ea                	jmp    10133e <file_read_all+0x4e>
  101354:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
  10135a:	8d bf 00 00 00 00    	lea    edi,[edi+0x0]

00101360 <file_write>:

int file_write(file_t *file, uint offset, void *buffer, uint n)
{
  101360:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
    if (file != NULL && file->fs->file_write !=NULL)
  101364:	85 c0                	test   eax,eax
  101366:	74 10                	je     101378 <file_write+0x18>
  101368:	8b 40 08             	mov    eax,DWORD PTR [eax+0x8]
  10136b:	8b 40 0c             	mov    eax,DWORD PTR [eax+0xc]
  10136e:	85 c0                	test   eax,eax
  101370:	74 06                	je     101378 <file_write+0x18>
    {
        return file->fs->file_write(file, offset, buffer, n);
  101372:	ff e0                	jmp    eax
  101374:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
    }

    return 0;
}
  101378:	31 c0                	xor    eax,eax
  10137a:	c3                   	ret    
  10137b:	66 90                	xchg   ax,ax
  10137d:	66 90                	xchg   ax,ax
  10137f:	90                   	nop

00101380 <load_module>:
#include "kernel/logger.h"
#include "kernel/modules.h"
#include "kernel/multiboot.h"

void load_module(multiboot_module_t *module)
{
  101380:	53                   	push   ebx
  101381:	83 ec 10             	sub    esp,0x10
  101384:	8b 5c 24 18          	mov    ebx,DWORD PTR [esp+0x18]
    log("Loading modules at 0x%x.", module);
  101388:	53                   	push   ebx
  101389:	68 b9 1e 11 00       	push   0x111eb9
  10138e:	e8 0d 03 00 00       	call   1016a0 <log>
    log("Module command line: '%s'.", module->cmdline);
  101393:	58                   	pop    eax
  101394:	5a                   	pop    edx
  101395:	ff 73 08             	push   DWORD PTR [ebx+0x8]
  101398:	68 d2 1e 11 00       	push   0x111ed2
  10139d:	e8 fe 02 00 00       	call   1016a0 <log>

    if (strcmp("ramdisk", (char*)module->cmdline) == 0)
  1013a2:	59                   	pop    ecx
  1013a3:	58                   	pop    eax
  1013a4:	ff 73 08             	push   DWORD PTR [ebx+0x8]
  1013a7:	68 ed 1e 11 00       	push   0x111eed
  1013ac:	e8 cf 47 00 00       	call   105b80 <strcmp>
  1013b1:	83 c4 10             	add    esp,0x10
  1013b4:	85 c0                	test   eax,eax
  1013b6:	74 18                	je     1013d0 <load_module+0x50>
    {
        ramdisk_load(module);
    }
    else
    {
        log("Unknow module '%s'!", module->cmdline);
  1013b8:	83 ec 08             	sub    esp,0x8
  1013bb:	ff 73 08             	push   DWORD PTR [ebx+0x8]
  1013be:	68 f5 1e 11 00       	push   0x111ef5
  1013c3:	e8 d8 02 00 00       	call   1016a0 <log>
    }
}
  1013c8:	83 c4 18             	add    esp,0x18
  1013cb:	5b                   	pop    ebx
  1013cc:	c3                   	ret    
  1013cd:	8d 76 00             	lea    esi,[esi+0x0]
        ramdisk_load(module);
  1013d0:	89 5c 24 10          	mov    DWORD PTR [esp+0x10],ebx
}
  1013d4:	83 c4 08             	add    esp,0x8
  1013d7:	5b                   	pop    ebx
        ramdisk_load(module);
  1013d8:	e9 f3 01 00 00       	jmp    1015d0 <ramdisk_load>
  1013dd:	8d 76 00             	lea    esi,[esi+0x0]

001013e0 <modules_get_end>:

uint modules_get_end(multiboot_info_t *minfo)
{
  1013e0:	8b 54 24 04          	mov    edx,DWORD PTR [esp+0x4]
    multiboot_module_t * module = (multiboot_module_t*)minfo->mods_addr;

    for(size_t i = 0; i < minfo->mods_count; i++)
  1013e4:	8b 4a 14             	mov    ecx,DWORD PTR [edx+0x14]
    multiboot_module_t * module = (multiboot_module_t*)minfo->mods_addr;
  1013e7:	8b 42 18             	mov    eax,DWORD PTR [edx+0x18]
    for(size_t i = 0; i < minfo->mods_count; i++)
  1013ea:	85 c9                	test   ecx,ecx
  1013ec:	74 0f                	je     1013fd <modules_get_end+0x1d>
  1013ee:	31 d2                	xor    edx,edx
    {
        module = (multiboot_module_t *)((u32)module->mod_end + 1);
  1013f0:	8b 40 04             	mov    eax,DWORD PTR [eax+0x4]
    for(size_t i = 0; i < minfo->mods_count; i++)
  1013f3:	83 c2 01             	add    edx,0x1
        module = (multiboot_module_t *)((u32)module->mod_end + 1);
  1013f6:	83 c0 01             	add    eax,0x1
    for(size_t i = 0; i < minfo->mods_count; i++)
  1013f9:	39 ca                	cmp    edx,ecx
  1013fb:	75 f3                	jne    1013f0 <modules_get_end+0x10>
    }

    return (uint)module;
}
  1013fd:	f3 c3                	repz ret 
  1013ff:	90                   	nop

00101400 <modules_setup>:

void modules_setup(multiboot_info_t *minfo)
{
  101400:	57                   	push   edi
  101401:	56                   	push   esi
    log("Kernel modules adresse: 0x%x.", minfo->mods_addr);

    
    multiboot_module_t * module = (multiboot_module_t*)minfo->mods_addr;

    for(size_t i = 0; i < minfo->mods_count; i++)
  101402:	31 f6                	xor    esi,esi
{
  101404:	53                   	push   ebx
  101405:	8b 7c 24 10          	mov    edi,DWORD PTR [esp+0x10]
    log("Bootloader: %s.", minfo->boot_loader_name);
  101409:	83 ec 08             	sub    esp,0x8
  10140c:	ff 77 40             	push   DWORD PTR [edi+0x40]
  10140f:	68 09 1f 11 00       	push   0x111f09
  101414:	e8 87 02 00 00       	call   1016a0 <log>
    log("Kernel modules count: %d.", minfo->mods_count);
  101419:	58                   	pop    eax
  10141a:	5a                   	pop    edx
  10141b:	ff 77 14             	push   DWORD PTR [edi+0x14]
  10141e:	68 19 1f 11 00       	push   0x111f19
  101423:	e8 78 02 00 00       	call   1016a0 <log>
    log("Kernel modules adresse: 0x%x.", minfo->mods_addr);
  101428:	59                   	pop    ecx
  101429:	5b                   	pop    ebx
  10142a:	ff 77 18             	push   DWORD PTR [edi+0x18]
  10142d:	68 33 1f 11 00       	push   0x111f33
  101432:	e8 69 02 00 00       	call   1016a0 <log>
    for(size_t i = 0; i < minfo->mods_count; i++)
  101437:	8b 47 14             	mov    eax,DWORD PTR [edi+0x14]
  10143a:	83 c4 10             	add    esp,0x10
    multiboot_module_t * module = (multiboot_module_t*)minfo->mods_addr;
  10143d:	8b 5f 18             	mov    ebx,DWORD PTR [edi+0x18]
    for(size_t i = 0; i < minfo->mods_count; i++)
  101440:	85 c0                	test   eax,eax
  101442:	74 5a                	je     10149e <modules_setup+0x9e>
  101444:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
    log("Loading modules at 0x%x.", module);
  101448:	83 ec 08             	sub    esp,0x8
  10144b:	53                   	push   ebx
  10144c:	68 b9 1e 11 00       	push   0x111eb9
  101451:	e8 4a 02 00 00       	call   1016a0 <log>
    log("Module command line: '%s'.", module->cmdline);
  101456:	58                   	pop    eax
  101457:	5a                   	pop    edx
  101458:	ff 73 08             	push   DWORD PTR [ebx+0x8]
  10145b:	68 d2 1e 11 00       	push   0x111ed2
  101460:	e8 3b 02 00 00       	call   1016a0 <log>
    if (strcmp("ramdisk", (char*)module->cmdline) == 0)
  101465:	59                   	pop    ecx
  101466:	58                   	pop    eax
  101467:	ff 73 08             	push   DWORD PTR [ebx+0x8]
  10146a:	68 ed 1e 11 00       	push   0x111eed
  10146f:	e8 0c 47 00 00       	call   105b80 <strcmp>
  101474:	83 c4 10             	add    esp,0x10
  101477:	85 c0                	test   eax,eax
  101479:	74 3d                	je     1014b8 <modules_setup+0xb8>
        log("Unknow module '%s'!", module->cmdline);
  10147b:	83 ec 08             	sub    esp,0x8
  10147e:	ff 73 08             	push   DWORD PTR [ebx+0x8]
    for(size_t i = 0; i < minfo->mods_count; i++)
  101481:	83 c6 01             	add    esi,0x1
        log("Unknow module '%s'!", module->cmdline);
  101484:	68 f5 1e 11 00       	push   0x111ef5
  101489:	e8 12 02 00 00       	call   1016a0 <log>
    {
        load_module(module);
        module = (multiboot_module_t *)((u32)module->mod_end + 1);
  10148e:	8b 5b 04             	mov    ebx,DWORD PTR [ebx+0x4]
    for(size_t i = 0; i < minfo->mods_count; i++)
  101491:	8b 47 14             	mov    eax,DWORD PTR [edi+0x14]
  101494:	83 c4 10             	add    esp,0x10
        module = (multiboot_module_t *)((u32)module->mod_end + 1);
  101497:	83 c3 01             	add    ebx,0x1
    for(size_t i = 0; i < minfo->mods_count; i++)
  10149a:	39 c6                	cmp    esi,eax
  10149c:	72 aa                	jb     101448 <modules_setup+0x48>
    }

    log("%d modules loaded!", minfo->mods_count);
  10149e:	83 ec 08             	sub    esp,0x8
  1014a1:	50                   	push   eax
  1014a2:	68 51 1f 11 00       	push   0x111f51
  1014a7:	e8 f4 01 00 00       	call   1016a0 <log>
  1014ac:	83 c4 10             	add    esp,0x10
  1014af:	5b                   	pop    ebx
  1014b0:	5e                   	pop    esi
  1014b1:	5f                   	pop    edi
  1014b2:	c3                   	ret    
  1014b3:	90                   	nop
  1014b4:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
        ramdisk_load(module);
  1014b8:	83 ec 0c             	sub    esp,0xc
    for(size_t i = 0; i < minfo->mods_count; i++)
  1014bb:	83 c6 01             	add    esi,0x1
        ramdisk_load(module);
  1014be:	53                   	push   ebx
  1014bf:	e8 0c 01 00 00       	call   1015d0 <ramdisk_load>
        module = (multiboot_module_t *)((u32)module->mod_end + 1);
  1014c4:	8b 5b 04             	mov    ebx,DWORD PTR [ebx+0x4]
    for(size_t i = 0; i < minfo->mods_count; i++)
  1014c7:	8b 47 14             	mov    eax,DWORD PTR [edi+0x14]
  1014ca:	83 c4 10             	add    esp,0x10
        module = (multiboot_module_t *)((u32)module->mod_end + 1);
  1014cd:	83 c3 01             	add    ebx,0x1
    for(size_t i = 0; i < minfo->mods_count; i++)
  1014d0:	39 f0                	cmp    eax,esi
  1014d2:	0f 87 70 ff ff ff    	ja     101448 <modules_setup+0x48>
  1014d8:	eb c4                	jmp    10149e <modules_setup+0x9e>
  1014da:	66 90                	xchg   ax,ax
  1014dc:	66 90                	xchg   ax,ax
  1014de:	66 90                	xchg   ax,ax

001014e0 <rd_file_close>:
}

void rd_file_close(file_t *file)
{
    UNUSED(file);
}
  1014e0:	f3 c3                	repz ret 
  1014e2:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
  1014e9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

001014f0 <rd_file_open>:
{
  1014f0:	83 c4 80             	add    esp,0xffffff80
    return tar_read(ramdisk, &block, file->inode);
  1014f3:	8b 84 24 84 00 00 00 	mov    eax,DWORD PTR [esp+0x84]
  1014fa:	ff 70 04             	push   DWORD PTR [eax+0x4]
  1014fd:	8d 44 24 0c          	lea    eax,[esp+0xc]
  101501:	50                   	push   eax
  101502:	ff 35 00 80 11 00    	push   DWORD PTR ds:0x118000
  101508:	e8 d3 64 00 00       	call   1079e0 <tar_read>
}
  10150d:	81 c4 8c 00 00 00    	add    esp,0x8c
    return tar_read(ramdisk, &block, file->inode);
  101513:	0f b6 c0             	movzx  eax,al
}
  101516:	c3                   	ret    
  101517:	89 f6                	mov    esi,esi
  101519:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00101520 <rd_file_stat>:
    UNUSED(file + offset + (int)buffer + n);
    return 0;
}

void rd_file_stat(file_t *file, fstat_t *stat)
{
  101520:	53                   	push   ebx
  101521:	83 ec 7c             	sub    esp,0x7c
    tar_block_t block;
    tar_read(ramdisk, &block, file->inode);
  101524:	8b 84 24 84 00 00 00 	mov    eax,DWORD PTR [esp+0x84]
{
  10152b:	8b 9c 24 88 00 00 00 	mov    ebx,DWORD PTR [esp+0x88]
    tar_read(ramdisk, &block, file->inode);
  101532:	ff 70 04             	push   DWORD PTR [eax+0x4]
  101535:	8d 44 24 0c          	lea    eax,[esp+0xc]
  101539:	50                   	push   eax
  10153a:	ff 35 00 80 11 00    	push   DWORD PTR ds:0x118000
  101540:	e8 9b 64 00 00       	call   1079e0 <tar_read>

    stat->size = block.size;
  101545:	8b 44 24 78          	mov    eax,DWORD PTR [esp+0x78]
    stat->read = 1;
  101549:	c7 43 04 01 00 00 00 	mov    DWORD PTR [ebx+0x4],0x1
    stat->write = 0;
  101550:	c7 43 08 00 00 00 00 	mov    DWORD PTR [ebx+0x8],0x0
    stat->size = block.size;
  101557:	89 03                	mov    DWORD PTR [ebx],eax
}
  101559:	81 c4 88 00 00 00    	add    esp,0x88
  10155f:	5b                   	pop    ebx
  101560:	c3                   	ret    
  101561:	eb 0d                	jmp    101570 <rd_file_read>
  101563:	90                   	nop
  101564:	90                   	nop
  101565:	90                   	nop
  101566:	90                   	nop
  101567:	90                   	nop
  101568:	90                   	nop
  101569:	90                   	nop
  10156a:	90                   	nop
  10156b:	90                   	nop
  10156c:	90                   	nop
  10156d:	90                   	nop
  10156e:	90                   	nop
  10156f:	90                   	nop

00101570 <rd_file_read>:
{
  101570:	56                   	push   esi
  101571:	53                   	push   ebx
  101572:	83 ec 78             	sub    esp,0x78
    tar_read(ramdisk, &block, file->inode);
  101575:	8b 84 24 84 00 00 00 	mov    eax,DWORD PTR [esp+0x84]
{
  10157c:	8b b4 24 88 00 00 00 	mov    esi,DWORD PTR [esp+0x88]
  101583:	8b 9c 24 90 00 00 00 	mov    ebx,DWORD PTR [esp+0x90]
    tar_read(ramdisk, &block, file->inode);
  10158a:	ff 70 04             	push   DWORD PTR [eax+0x4]
  10158d:	8d 44 24 0c          	lea    eax,[esp+0xc]
  101591:	50                   	push   eax
  101592:	ff 35 00 80 11 00    	push   DWORD PTR ds:0x118000
  101598:	e8 43 64 00 00       	call   1079e0 <tar_read>
    n = min(block.size - min(offset, block.size), n);
  10159d:	8b 44 24 78          	mov    eax,DWORD PTR [esp+0x78]
  1015a1:	83 c4 0c             	add    esp,0xc
  1015a4:	89 c2                	mov    edx,eax
  1015a6:	29 f2                	sub    edx,esi
  1015a8:	39 d0                	cmp    eax,edx
  1015aa:	0f 47 c6             	cmova  eax,esi
  1015ad:	39 d8                	cmp    eax,ebx
  1015af:	0f 46 d8             	cmovbe ebx,eax
    memcpy(buffer, block.data + offset, n);
  1015b2:	53                   	push   ebx
  1015b3:	03 74 24 74          	add    esi,DWORD PTR [esp+0x74]
  1015b7:	56                   	push   esi
  1015b8:	ff b4 24 94 00 00 00 	push   DWORD PTR [esp+0x94]
  1015bf:	e8 4c 40 00 00       	call   105610 <memcpy>
}
  1015c4:	81 c4 84 00 00 00    	add    esp,0x84
  1015ca:	89 d8                	mov    eax,ebx
  1015cc:	5b                   	pop    ebx
  1015cd:	5e                   	pop    esi
  1015ce:	c3                   	ret    
  1015cf:	90                   	nop

001015d0 <ramdisk_load>:
{
  1015d0:	56                   	push   esi
  1015d1:	53                   	push   ebx
  1015d2:	83 ec 7c             	sub    esp,0x7c
  1015d5:	8b 9c 24 88 00 00 00 	mov    ebx,DWORD PTR [esp+0x88]
    log("Loading ramdisk at 0x%x...", module->mod_start);
  1015dc:	ff 33                	push   DWORD PTR [ebx]
  1015de:	68 64 1f 11 00       	push   0x111f64
  1015e3:	e8 b8 00 00 00       	call   1016a0 <log>
    ramdisk = (void *)module->mod_start;
  1015e8:	8b 03                	mov    eax,DWORD PTR [ebx]
    for (size_t i = 0; tar_read(ramdisk, &block, i); i++)
  1015ea:	83 c4 10             	add    esp,0x10
    ramdisk_fs.file_open = rd_file_open;
  1015ed:	c7 05 04 80 11 00 f0 	mov    DWORD PTR ds:0x118004,0x1014f0
  1015f4:	14 10 00 
    ramdisk_fs.file_close = rd_file_close;
  1015f7:	c7 05 08 80 11 00 e0 	mov    DWORD PTR ds:0x118008,0x1014e0
  1015fe:	14 10 00 
    ramdisk_fs.file_read = rd_file_read;
  101601:	c7 05 0c 80 11 00 70 	mov    DWORD PTR ds:0x11800c,0x101570
  101608:	15 10 00 
    for (size_t i = 0; tar_read(ramdisk, &block, i); i++)
  10160b:	31 db                	xor    ebx,ebx
    ramdisk_fs.file_stat = rd_file_stat;
  10160d:	c7 05 14 80 11 00 20 	mov    DWORD PTR ds:0x118014,0x101520
  101614:	15 10 00 
  101617:	8d 74 24 04          	lea    esi,[esp+0x4]
    ramdisk = (void *)module->mod_start;
  10161b:	a3 00 80 11 00       	mov    ds:0x118000,eax
    for (size_t i = 0; tar_read(ramdisk, &block, i); i++)
  101620:	eb 24                	jmp    101646 <ramdisk_load+0x76>
  101622:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
            file_create(NULL, block.name, &ramdisk_fs, 0, i);
  101628:	83 ec 0c             	sub    esp,0xc
  10162b:	53                   	push   ebx
  10162c:	6a 00                	push   0x0
  10162e:	68 04 80 11 00       	push   0x118004
  101633:	56                   	push   esi
  101634:	6a 00                	push   0x0
  101636:	e8 75 f7 ff ff       	call   100db0 <file_create>
  10163b:	83 c4 20             	add    esp,0x20
  10163e:	a1 00 80 11 00       	mov    eax,ds:0x118000
    for (size_t i = 0; tar_read(ramdisk, &block, i); i++)
  101643:	83 c3 01             	add    ebx,0x1
  101646:	83 ec 04             	sub    esp,0x4
  101649:	53                   	push   ebx
  10164a:	56                   	push   esi
  10164b:	50                   	push   eax
  10164c:	e8 8f 63 00 00       	call   1079e0 <tar_read>
  101651:	83 c4 10             	add    esp,0x10
  101654:	84 c0                	test   al,al
  101656:	74 28                	je     101680 <ramdisk_load+0xb0>
        if (block.name[strlen(block.name) - 1] == '/')
  101658:	83 ec 0c             	sub    esp,0xc
  10165b:	56                   	push   esi
  10165c:	e8 5f 4a 00 00       	call   1060c0 <strlen>
  101661:	83 c4 10             	add    esp,0x10
  101664:	80 7c 04 03 2f       	cmp    BYTE PTR [esp+eax*1+0x3],0x2f
  101669:	75 bd                	jne    101628 <ramdisk_load+0x58>
            directory_create(NULL, block.name, 0);
  10166b:	83 ec 04             	sub    esp,0x4
  10166e:	6a 00                	push   0x0
  101670:	56                   	push   esi
  101671:	6a 00                	push   0x0
  101673:	e8 a8 f2 ff ff       	call   100920 <directory_create>
  101678:	83 c4 10             	add    esp,0x10
  10167b:	eb c1                	jmp    10163e <ramdisk_load+0x6e>
  10167d:	8d 76 00             	lea    esi,[esi+0x0]
}
  101680:	83 c4 74             	add    esp,0x74
  101683:	5b                   	pop    ebx
  101684:	5e                   	pop    esi
  101685:	c3                   	ret    
  101686:	8d 76 00             	lea    esi,[esi+0x0]
  101689:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00101690 <rd_file_write>:
}
  101690:	31 c0                	xor    eax,eax
  101692:	c3                   	ret    
  101693:	66 90                	xchg   ax,ax
  101695:	66 90                	xchg   ax,ax
  101697:	66 90                	xchg   ax,ax
  101699:	66 90                	xchg   ax,ax
  10169b:	66 90                	xchg   ax,ax
  10169d:	66 90                	xchg   ax,ax
  10169f:	90                   	nop

001016a0 <log>:
#include <stdio.h>
#include "kernel/logger.h"
#include "kernel/time.h"

void log(const char *message, ...)
{
  1016a0:	56                   	push   esi
  1016a1:	53                   	push   ebx
  1016a2:	83 ec 10             	sub    esp,0x10
    va_list va;
    va_start(va, message);
    
    printf("[%d:%d:%d] ", time_get(TIME_HOUR), time_get(TIME_MINUTE), time_get(TIME_SECOND));
  1016a5:	6a 00                	push   0x0
  1016a7:	e8 a4 38 00 00       	call   104f50 <time_get>
  1016ac:	c7 04 24 02 00 00 00 	mov    DWORD PTR [esp],0x2
  1016b3:	89 c6                	mov    esi,eax
  1016b5:	e8 96 38 00 00       	call   104f50 <time_get>
  1016ba:	89 c3                	mov    ebx,eax
  1016bc:	c7 04 24 04 00 00 00 	mov    DWORD PTR [esp],0x4
  1016c3:	e8 88 38 00 00       	call   104f50 <time_get>
  1016c8:	56                   	push   esi
  1016c9:	53                   	push   ebx
  1016ca:	50                   	push   eax
  1016cb:	68 7f 1f 11 00       	push   0x111f7f
  1016d0:	e8 9b 4f 00 00       	call   106670 <printf>
    vprintf(message, va);
  1016d5:	83 c4 18             	add    esp,0x18
  1016d8:	8d 44 24 1c          	lea    eax,[esp+0x1c]
  1016dc:	50                   	push   eax
  1016dd:	ff 74 24 1c          	push   DWORD PTR [esp+0x1c]
  1016e1:	e8 ba 51 00 00       	call   1068a0 <vprintf>
    putchar('\n');
  1016e6:	c7 04 24 0a 00 00 00 	mov    DWORD PTR [esp],0xa
  1016ed:	e8 5e 61 00 00       	call   107850 <putchar>
    
    va_end(va);
  1016f2:	83 c4 14             	add    esp,0x14
  1016f5:	5b                   	pop    ebx
  1016f6:	5e                   	pop    esi
  1016f7:	c3                   	ret    
  1016f8:	66 90                	xchg   ax,ax
  1016fa:	66 90                	xchg   ax,ax
  1016fc:	66 90                	xchg   ax,ax
  1016fe:	66 90                	xchg   ax,ax

00101700 <shedule>:
{
    return memory_unmap(process->pdir, addr, count);
}

esp_t shedule(esp_t esp, context_t *context)
{
  101700:	53                   	push   ebx
  101701:	83 ec 10             	sub    esp,0x10
    ticks++;

    UNUSED(context);

    // Save the old context
    running->esp = esp;
  101704:	a1 1c 80 11 00       	mov    eax,ds:0x11801c
    ticks++;
  101709:	83 05 1c ae 23 00 01 	add    DWORD PTR ds:0x23ae1c,0x1
    running->esp = esp;
  101710:	8b 54 24 18          	mov    edx,DWORD PTR [esp+0x18]
  101714:	89 50 08             	mov    DWORD PTR [eax+0x8],edx

    if (!list_containe(dead, (int)running))
  101717:	50                   	push   eax
  101718:	ff 35 20 80 11 00    	push   DWORD PTR ds:0x118020
  10171e:	e8 ad 6b 00 00       	call   1082d0 <list_containe>
  101723:	83 c4 10             	add    esp,0x10
  101726:	85 c0                	test   eax,eax
  101728:	74 76                	je     1017a0 <shedule+0xa0>
    {
        list_pushback(waiting, (int)running);
    }

    FOREACH(i, dead)
  10172a:	a1 20 80 11 00       	mov    eax,ds:0x118020
  10172f:	8b 58 04             	mov    ebx,DWORD PTR [eax+0x4]
  101732:	85 db                	test   ebx,ebx
  101734:	74 34                	je     10176a <shedule+0x6a>
  101736:	8d 76 00             	lea    esi,[esi+0x0]
  101739:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
    {
        thread_t *deadthread = (thread_t *)i;
        list_remove(waiting, (int)deadthread);
  101740:	83 ec 08             	sub    esp,0x8
  101743:	53                   	push   ebx
  101744:	ff 35 24 80 11 00    	push   DWORD PTR ds:0x118024
  10174a:	e8 f1 6a 00 00       	call   108240 <list_remove>
    free(thread->stack);
  10174f:	58                   	pop    eax
  101750:	ff 73 04             	push   DWORD PTR [ebx+0x4]
  101753:	e8 18 5d 00 00       	call   107470 <free>
    free(thread);
  101758:	89 1c 24             	mov    DWORD PTR [esp],ebx
  10175b:	e8 10 5d 00 00       	call   107470 <free>
    FOREACH(i, dead)
  101760:	8b 5b 04             	mov    ebx,DWORD PTR [ebx+0x4]
  101763:	83 c4 10             	add    esp,0x10
  101766:	85 db                	test   ebx,ebx
  101768:	75 d6                	jne    101740 <shedule+0x40>
        thread_free(deadthread);
    }

    list_pop(waiting, (int *)&running);
  10176a:	83 ec 08             	sub    esp,0x8
  10176d:	68 1c 80 11 00       	push   0x11801c
  101772:	ff 35 24 80 11 00    	push   DWORD PTR ds:0x118024
  101778:	e8 93 69 00 00       	call   108110 <list_pop>
    set_kernel_stack((uint)running->stack + STACK_SIZE);
  10177d:	a1 1c 80 11 00       	mov    eax,ds:0x11801c
  101782:	8b 40 04             	mov    eax,DWORD PTR [eax+0x4]
  101785:	05 00 10 00 00       	add    eax,0x1000
  10178a:	89 04 24             	mov    DWORD PTR [esp],eax
  10178d:	e8 fe 30 00 00       	call   104890 <set_kernel_stack>

    // Load the new context
    return running->esp;
  101792:	a1 1c 80 11 00       	mov    eax,ds:0x11801c
  101797:	8b 40 08             	mov    eax,DWORD PTR [eax+0x8]
}
  10179a:	83 c4 18             	add    esp,0x18
  10179d:	5b                   	pop    ebx
  10179e:	c3                   	ret    
  10179f:	90                   	nop
        list_pushback(waiting, (int)running);
  1017a0:	83 ec 08             	sub    esp,0x8
  1017a3:	ff 35 1c 80 11 00    	push   DWORD PTR ds:0x11801c
  1017a9:	ff 35 24 80 11 00    	push   DWORD PTR ds:0x118024
  1017af:	e8 cc 69 00 00       	call   108180 <list_pushback>
  1017b4:	83 c4 10             	add    esp,0x10
  1017b7:	e9 6e ff ff ff       	jmp    10172a <shedule+0x2a>
  1017bc:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]

001017c0 <thread_alloc>:
{
  1017c0:	56                   	push   esi
  1017c1:	53                   	push   ebx
  1017c2:	83 ec 10             	sub    esp,0x10
  1017c5:	8b 74 24 1c          	mov    esi,DWORD PTR [esp+0x1c]
    thread_t *thread = (thread_t *)malloc(sizeof(thread_t));
  1017c9:	6a 14                	push   0x14
  1017cb:	e8 d0 57 00 00       	call   106fa0 <malloc>
    memset(thread, 0, sizeof(thread_t));
  1017d0:	83 c4 0c             	add    esp,0xc
    thread_t *thread = (thread_t *)malloc(sizeof(thread_t));
  1017d3:	89 c3                	mov    ebx,eax
    memset(thread, 0, sizeof(thread_t));
  1017d5:	6a 14                	push   0x14
  1017d7:	6a 00                	push   0x0
  1017d9:	50                   	push   eax
  1017da:	e8 f1 3e 00 00       	call   1056d0 <memset>
    atomic_begin();
  1017df:	e8 5c 66 00 00       	call   107e40 <atomic_begin>
    thread->id = TID++;
  1017e4:	a1 04 ae 23 00       	mov    eax,ds:0x23ae04
  1017e9:	8d 50 01             	lea    edx,[eax+0x1]
  1017ec:	89 15 04 ae 23 00    	mov    DWORD PTR ds:0x23ae04,edx
  1017f2:	89 03                	mov    DWORD PTR [ebx],eax
    atomic_end();
  1017f4:	e8 67 66 00 00       	call   107e60 <atomic_end>
    thread->stack = malloc(STACK_SIZE);
  1017f9:	c7 04 24 00 10 00 00 	mov    DWORD PTR [esp],0x1000
  101800:	e8 9b 57 00 00       	call   106fa0 <malloc>
    if (user)
  101805:	83 c4 10             	add    esp,0x10
  101808:	8b 4c 24 14          	mov    ecx,DWORD PTR [esp+0x14]
    thread->esp -= sizeof(context_t);
  10180c:	8d 90 bc 0f 00 00    	lea    edx,[eax+0xfbc]
    thread->stack = malloc(STACK_SIZE);
  101812:	89 43 04             	mov    DWORD PTR [ebx+0x4],eax
    thread->entry = entry;
  101815:	89 73 0c             	mov    DWORD PTR [ebx+0xc],esi
    thread->esp -= sizeof(context_t);
  101818:	89 53 08             	mov    DWORD PTR [ebx+0x8],edx
    context->eflags = 0x202;
  10181b:	c7 42 40 02 02 00 00 	mov    DWORD PTR [edx+0x40],0x202
    if (user)
  101822:	85 c9                	test   ecx,ecx
    context->eip = (u32)entry;
  101824:	89 72 38             	mov    DWORD PTR [edx+0x38],esi
    if (user)
  101827:	75 37                	jne    101860 <thread_alloc+0xa0>
        context->cs = 0x08;
  101829:	c7 42 3c 08 00 00 00 	mov    DWORD PTR [edx+0x3c],0x8
        context->ds = 0x10;
  101830:	c7 42 0c 10 00 00 00 	mov    DWORD PTR [edx+0xc],0x10
        context->es = 0x10;
  101837:	c7 42 08 10 00 00 00 	mov    DWORD PTR [edx+0x8],0x10
        context->fs = 0x10;
  10183e:	c7 42 04 10 00 00 00 	mov    DWORD PTR [edx+0x4],0x10
        context->gs = 0x10;
  101845:	c7 80 bc 0f 00 00 10 	mov    DWORD PTR [eax+0xfbc],0x10
  10184c:	00 00 00 
}
  10184f:	83 c4 04             	add    esp,0x4
  101852:	89 d8                	mov    eax,ebx
  101854:	5b                   	pop    ebx
  101855:	5e                   	pop    esi
  101856:	c3                   	ret    
  101857:	89 f6                	mov    esi,esi
  101859:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
        context->cs = 0x18;
  101860:	c7 42 3c 18 00 00 00 	mov    DWORD PTR [edx+0x3c],0x18
        context->ds = 0x20;
  101867:	c7 42 0c 20 00 00 00 	mov    DWORD PTR [edx+0xc],0x20
        context->es = 0x20;
  10186e:	c7 42 08 20 00 00 00 	mov    DWORD PTR [edx+0x8],0x20
        context->fs = 0x20;
  101875:	c7 42 04 20 00 00 00 	mov    DWORD PTR [edx+0x4],0x20
        context->gs = 0x20;
  10187c:	c7 80 bc 0f 00 00 20 	mov    DWORD PTR [eax+0xfbc],0x20
  101883:	00 00 00 
}
  101886:	83 c4 04             	add    esp,0x4
  101889:	89 d8                	mov    eax,ebx
  10188b:	5b                   	pop    ebx
  10188c:	5e                   	pop    esi
  10188d:	c3                   	ret    
  10188e:	66 90                	xchg   ax,ax

00101890 <thread_free>:
{
  101890:	53                   	push   ebx
  101891:	83 ec 14             	sub    esp,0x14
  101894:	8b 5c 24 1c          	mov    ebx,DWORD PTR [esp+0x1c]
    free(thread->stack);
  101898:	ff 73 04             	push   DWORD PTR [ebx+0x4]
  10189b:	e8 d0 5b 00 00       	call   107470 <free>
    free(thread);
  1018a0:	89 5c 24 20          	mov    DWORD PTR [esp+0x20],ebx
}
  1018a4:	83 c4 18             	add    esp,0x18
  1018a7:	5b                   	pop    ebx
    free(thread);
  1018a8:	e9 c3 5b 00 00       	jmp    107470 <free>
  1018ad:	8d 76 00             	lea    esi,[esi+0x0]

001018b0 <process_alloc>:
{
  1018b0:	56                   	push   esi
  1018b1:	53                   	push   ebx
  1018b2:	83 ec 10             	sub    esp,0x10
  1018b5:	8b 74 24 20          	mov    esi,DWORD PTR [esp+0x20]
    process_t *process = (process_t *)malloc(sizeof(process_t));
  1018b9:	68 90 00 00 00       	push   0x90
  1018be:	e8 dd 56 00 00       	call   106fa0 <malloc>
  1018c3:	89 c3                	mov    ebx,eax
    process->id = PID++;
  1018c5:	a1 08 ae 23 00       	mov    eax,ds:0x23ae08
    strncpy(process->name, name, PROCNAME_SIZE);
  1018ca:	83 c4 0c             	add    esp,0xc
    process->id = PID++;
  1018cd:	8d 50 01             	lea    edx,[eax+0x1]
  1018d0:	89 15 08 ae 23 00    	mov    DWORD PTR ds:0x23ae08,edx
  1018d6:	89 03                	mov    DWORD PTR [ebx],eax
    strncpy(process->name, name, PROCNAME_SIZE);
  1018d8:	8d 43 04             	lea    eax,[ebx+0x4]
  1018db:	68 80 00 00 00       	push   0x80
  1018e0:	ff 74 24 18          	push   DWORD PTR [esp+0x18]
  1018e4:	50                   	push   eax
  1018e5:	e8 56 45 00 00       	call   105e40 <strncpy>
    process->user = user;
  1018ea:	89 f0                	mov    eax,esi
  1018ec:	88 83 84 00 00 00    	mov    BYTE PTR [ebx+0x84],al
    process->threads = list_alloc();
  1018f2:	e8 c9 65 00 00       	call   107ec0 <list_alloc>
    if (user)
  1018f7:	83 c4 10             	add    esp,0x10
  1018fa:	85 f6                	test   esi,esi
    process->threads = list_alloc();
  1018fc:	89 83 88 00 00 00    	mov    DWORD PTR [ebx+0x88],eax
    if (user)
  101902:	75 1c                	jne    101920 <process_alloc+0x70>
        process->pdir = memory_kpdir();
  101904:	e8 77 13 00 00       	call   102c80 <memory_kpdir>
  101909:	89 83 8c 00 00 00    	mov    DWORD PTR [ebx+0x8c],eax
}
  10190f:	83 c4 04             	add    esp,0x4
  101912:	89 d8                	mov    eax,ebx
  101914:	5b                   	pop    ebx
  101915:	5e                   	pop    esi
  101916:	c3                   	ret    
  101917:	89 f6                	mov    esi,esi
  101919:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
        process->pdir = memory_alloc_pdir();
  101920:	e8 7b 15 00 00       	call   102ea0 <memory_alloc_pdir>
  101925:	89 83 8c 00 00 00    	mov    DWORD PTR [ebx+0x8c],eax
}
  10192b:	83 c4 04             	add    esp,0x4
  10192e:	89 d8                	mov    eax,ebx
  101930:	5b                   	pop    ebx
  101931:	5e                   	pop    esi
  101932:	c3                   	ret    
  101933:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
  101939:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00101940 <process_free>:
{
  101940:	56                   	push   esi
  101941:	53                   	push   ebx
  101942:	83 ec 04             	sub    esp,0x4
  101945:	8b 5c 24 10          	mov    ebx,DWORD PTR [esp+0x10]
    atomic_begin();
  101949:	e8 f2 64 00 00       	call   107e40 <atomic_begin>
    if (process->pdir != memory_kpdir())
  10194e:	8b b3 8c 00 00 00    	mov    esi,DWORD PTR [ebx+0x8c]
  101954:	e8 27 13 00 00       	call   102c80 <memory_kpdir>
  101959:	39 c6                	cmp    esi,eax
  10195b:	74 11                	je     10196e <process_free+0x2e>
        memory_free_pdir(process->pdir);
  10195d:	83 ec 0c             	sub    esp,0xc
  101960:	ff b3 8c 00 00 00    	push   DWORD PTR [ebx+0x8c]
  101966:	e8 55 16 00 00       	call   102fc0 <memory_free_pdir>
  10196b:	83 c4 10             	add    esp,0x10
    list_free(process->threads);
  10196e:	83 ec 0c             	sub    esp,0xc
  101971:	ff b3 88 00 00 00    	push   DWORD PTR [ebx+0x88]
  101977:	e8 74 65 00 00       	call   107ef0 <list_free>
    free(process);
  10197c:	89 1c 24             	mov    DWORD PTR [esp],ebx
  10197f:	e8 ec 5a 00 00       	call   107470 <free>
}
  101984:	83 c4 14             	add    esp,0x14
  101987:	5b                   	pop    ebx
  101988:	5e                   	pop    esi
    atomic_end();
  101989:	e9 d2 64 00 00       	jmp    107e60 <atomic_end>
  10198e:	66 90                	xchg   ax,ax

00101990 <tasking_setup>:
{
  101990:	53                   	push   ebx
  101991:	83 ec 08             	sub    esp,0x8
    waiting = list_alloc();
  101994:	e8 27 65 00 00       	call   107ec0 <list_alloc>
  101999:	a3 24 80 11 00       	mov    ds:0x118024,eax
    dead = list_alloc();
  10199e:	e8 1d 65 00 00       	call   107ec0 <list_alloc>
  1019a3:	a3 20 80 11 00       	mov    ds:0x118020,eax
    process = list_alloc();
  1019a8:	e8 13 65 00 00       	call   107ec0 <list_alloc>
    thread_t *thread = (thread_t *)malloc(sizeof(thread_t));
  1019ad:	83 ec 0c             	sub    esp,0xc
    process = list_alloc();
  1019b0:	a3 18 80 11 00       	mov    ds:0x118018,eax
    thread_t *thread = (thread_t *)malloc(sizeof(thread_t));
  1019b5:	6a 14                	push   0x14
  1019b7:	e8 e4 55 00 00       	call   106fa0 <malloc>
    memset(thread, 0, sizeof(thread_t));
  1019bc:	83 c4 0c             	add    esp,0xc
    thread_t *thread = (thread_t *)malloc(sizeof(thread_t));
  1019bf:	89 c3                	mov    ebx,eax
    memset(thread, 0, sizeof(thread_t));
  1019c1:	6a 14                	push   0x14
  1019c3:	6a 00                	push   0x0
  1019c5:	50                   	push   eax
  1019c6:	e8 05 3d 00 00       	call   1056d0 <memset>
    atomic_begin();
  1019cb:	e8 70 64 00 00       	call   107e40 <atomic_begin>
    thread->id = TID++;
  1019d0:	a1 04 ae 23 00       	mov    eax,ds:0x23ae04
  1019d5:	8d 50 01             	lea    edx,[eax+0x1]
  1019d8:	89 15 04 ae 23 00    	mov    DWORD PTR ds:0x23ae04,edx
  1019de:	89 03                	mov    DWORD PTR [ebx],eax
    atomic_end();
  1019e0:	e8 7b 64 00 00       	call   107e60 <atomic_end>
    thread->stack = malloc(STACK_SIZE);
  1019e5:	c7 04 24 00 10 00 00 	mov    DWORD PTR [esp],0x1000
  1019ec:	e8 af 55 00 00       	call   106fa0 <malloc>
    thread->esp -= sizeof(context_t);
  1019f1:	8d 90 bc 0f 00 00    	lea    edx,[eax+0xfbc]
    thread->stack = malloc(STACK_SIZE);
  1019f7:	89 43 04             	mov    DWORD PTR [ebx+0x4],eax
    thread->entry = entry;
  1019fa:	c7 43 0c 00 00 00 00 	mov    DWORD PTR [ebx+0xc],0x0
    thread->esp -= sizeof(context_t);
  101a01:	89 53 08             	mov    DWORD PTR [ebx+0x8],edx
    context->eflags = 0x202;
  101a04:	c7 80 fc 0f 00 00 02 	mov    DWORD PTR [eax+0xffc],0x202
  101a0b:	02 00 00 
    context->eip = (u32)entry;
  101a0e:	c7 80 f4 0f 00 00 00 	mov    DWORD PTR [eax+0xff4],0x0
  101a15:	00 00 00 
        context->cs = 0x08;
  101a18:	c7 80 f8 0f 00 00 08 	mov    DWORD PTR [eax+0xff8],0x8
  101a1f:	00 00 00 
        context->ds = 0x10;
  101a22:	c7 80 c8 0f 00 00 10 	mov    DWORD PTR [eax+0xfc8],0x10
  101a29:	00 00 00 
        context->es = 0x10;
  101a2c:	c7 80 c4 0f 00 00 10 	mov    DWORD PTR [eax+0xfc4],0x10
  101a33:	00 00 00 
        context->fs = 0x10;
  101a36:	c7 80 c0 0f 00 00 10 	mov    DWORD PTR [eax+0xfc0],0x10
  101a3d:	00 00 00 
        context->gs = 0x10;
  101a40:	c7 80 bc 0f 00 00 10 	mov    DWORD PTR [eax+0xfbc],0x10
  101a47:	00 00 00 
    atomic_begin();
  101a4a:	e8 f1 63 00 00       	call   107e40 <atomic_begin>
    if (running)
  101a4f:	a1 1c 80 11 00       	mov    eax,ds:0x11801c
  101a54:	83 c4 10             	add    esp,0x10
  101a57:	85 c0                	test   eax,eax
  101a59:	75 25                	jne    101a80 <tasking_setup+0xf0>
        running = thread;
  101a5b:	89 1d 1c 80 11 00    	mov    DWORD PTR ds:0x11801c,ebx
    atomic_end();
  101a61:	e8 fa 63 00 00       	call   107e60 <atomic_end>
    irq_register(0, (irq_handler_t)&shedule);
  101a66:	83 ec 08             	sub    esp,0x8
  101a69:	68 00 17 10 00       	push   0x101700
  101a6e:	6a 00                	push   0x0
  101a70:	e8 0b 29 00 00       	call   104380 <irq_register>
}
  101a75:	83 c4 18             	add    esp,0x18
  101a78:	5b                   	pop    ebx
  101a79:	c3                   	ret    
  101a7a:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
        list_pushback(waiting, (int)thread);
  101a80:	83 ec 08             	sub    esp,0x8
  101a83:	53                   	push   ebx
  101a84:	ff 35 24 80 11 00    	push   DWORD PTR ds:0x118024
  101a8a:	e8 f1 66 00 00       	call   108180 <list_pushback>
  101a8f:	83 c4 10             	add    esp,0x10
  101a92:	eb cd                	jmp    101a61 <tasking_setup+0xd1>
  101a94:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
  101a9a:	8d bf 00 00 00 00    	lea    edi,[edi+0x0]

00101aa0 <thread_create>:
{
  101aa0:	56                   	push   esi
  101aa1:	53                   	push   ebx
  101aa2:	83 ec 10             	sub    esp,0x10
  101aa5:	8b 74 24 1c          	mov    esi,DWORD PTR [esp+0x1c]
    thread_t *thread = (thread_t *)malloc(sizeof(thread_t));
  101aa9:	6a 14                	push   0x14
  101aab:	e8 f0 54 00 00       	call   106fa0 <malloc>
    memset(thread, 0, sizeof(thread_t));
  101ab0:	83 c4 0c             	add    esp,0xc
    thread_t *thread = (thread_t *)malloc(sizeof(thread_t));
  101ab3:	89 c3                	mov    ebx,eax
    memset(thread, 0, sizeof(thread_t));
  101ab5:	6a 14                	push   0x14
  101ab7:	6a 00                	push   0x0
  101ab9:	50                   	push   eax
  101aba:	e8 11 3c 00 00       	call   1056d0 <memset>
    atomic_begin();
  101abf:	e8 7c 63 00 00       	call   107e40 <atomic_begin>
    thread->id = TID++;
  101ac4:	a1 04 ae 23 00       	mov    eax,ds:0x23ae04
  101ac9:	8d 50 01             	lea    edx,[eax+0x1]
  101acc:	89 15 04 ae 23 00    	mov    DWORD PTR ds:0x23ae04,edx
  101ad2:	89 03                	mov    DWORD PTR [ebx],eax
    atomic_end();
  101ad4:	e8 87 63 00 00       	call   107e60 <atomic_end>
    thread->stack = malloc(STACK_SIZE);
  101ad9:	c7 04 24 00 10 00 00 	mov    DWORD PTR [esp],0x1000
  101ae0:	e8 bb 54 00 00       	call   106fa0 <malloc>
    thread->esp -= sizeof(context_t);
  101ae5:	8d 90 bc 0f 00 00    	lea    edx,[eax+0xfbc]
    thread->stack = malloc(STACK_SIZE);
  101aeb:	89 43 04             	mov    DWORD PTR [ebx+0x4],eax
    thread->entry = entry;
  101aee:	89 73 0c             	mov    DWORD PTR [ebx+0xc],esi
    thread->esp -= sizeof(context_t);
  101af1:	89 53 08             	mov    DWORD PTR [ebx+0x8],edx
    context->eflags = 0x202;
  101af4:	c7 80 fc 0f 00 00 02 	mov    DWORD PTR [eax+0xffc],0x202
  101afb:	02 00 00 
    context->eip = (u32)entry;
  101afe:	89 b0 f4 0f 00 00    	mov    DWORD PTR [eax+0xff4],esi
        context->cs = 0x08;
  101b04:	c7 80 f8 0f 00 00 08 	mov    DWORD PTR [eax+0xff8],0x8
  101b0b:	00 00 00 
        context->ds = 0x10;
  101b0e:	c7 80 c8 0f 00 00 10 	mov    DWORD PTR [eax+0xfc8],0x10
  101b15:	00 00 00 
        context->es = 0x10;
  101b18:	c7 80 c4 0f 00 00 10 	mov    DWORD PTR [eax+0xfc4],0x10
  101b1f:	00 00 00 
        context->fs = 0x10;
  101b22:	c7 80 c0 0f 00 00 10 	mov    DWORD PTR [eax+0xfc0],0x10
  101b29:	00 00 00 
        context->gs = 0x10;
  101b2c:	c7 80 bc 0f 00 00 10 	mov    DWORD PTR [eax+0xfbc],0x10
  101b33:	00 00 00 
    atomic_begin();
  101b36:	e8 05 63 00 00       	call   107e40 <atomic_begin>
    if (running)
  101b3b:	a1 1c 80 11 00       	mov    eax,ds:0x11801c
  101b40:	83 c4 10             	add    esp,0x10
  101b43:	85 c0                	test   eax,eax
  101b45:	75 19                	jne    101b60 <thread_create+0xc0>
        running = thread;
  101b47:	89 1d 1c 80 11 00    	mov    DWORD PTR ds:0x11801c,ebx
    atomic_end();
  101b4d:	e8 0e 63 00 00       	call   107e60 <atomic_end>
}
  101b52:	83 c4 04             	add    esp,0x4
  101b55:	89 d8                	mov    eax,ebx
  101b57:	5b                   	pop    ebx
  101b58:	5e                   	pop    esi
  101b59:	c3                   	ret    
  101b5a:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
        list_pushback(waiting, (int)thread);
  101b60:	83 ec 08             	sub    esp,0x8
  101b63:	53                   	push   ebx
  101b64:	ff 35 24 80 11 00    	push   DWORD PTR ds:0x118024
  101b6a:	e8 11 66 00 00       	call   108180 <list_pushback>
  101b6f:	83 c4 10             	add    esp,0x10
    atomic_end();
  101b72:	e8 e9 62 00 00       	call   107e60 <atomic_end>
}
  101b77:	83 c4 04             	add    esp,0x4
  101b7a:	89 d8                	mov    eax,ebx
  101b7c:	5b                   	pop    ebx
  101b7d:	5e                   	pop    esi
  101b7e:	c3                   	ret    
  101b7f:	90                   	nop

00101b80 <thread_cancel>:
{
  101b80:	53                   	push   ebx
  101b81:	83 ec 08             	sub    esp,0x8
  101b84:	8b 5c 24 10          	mov    ebx,DWORD PTR [esp+0x10]
    atomic_begin();
  101b88:	e8 b3 62 00 00       	call   107e40 <atomic_begin>
  101b8d:	31 c0                	xor    eax,eax
    if (thread == NULL)
  101b8f:	85 db                	test   ebx,ebx
  101b91:	74 20                	je     101bb3 <thread_cancel+0x33>
    if (!list_containe(dead, (int)thread))
  101b93:	83 ec 08             	sub    esp,0x8
  101b96:	53                   	push   ebx
  101b97:	ff 35 20 80 11 00    	push   DWORD PTR ds:0x118020
  101b9d:	e8 2e 67 00 00       	call   1082d0 <list_containe>
  101ba2:	83 c4 10             	add    esp,0x10
  101ba5:	85 c0                	test   eax,eax
  101ba7:	74 17                	je     101bc0 <thread_cancel+0x40>
    atomic_end();
  101ba9:	e8 b2 62 00 00       	call   107e60 <atomic_end>
  101bae:	b8 01 00 00 00       	mov    eax,0x1
}
  101bb3:	83 c4 08             	add    esp,0x8
  101bb6:	5b                   	pop    ebx
  101bb7:	c3                   	ret    
  101bb8:	90                   	nop
  101bb9:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
        list_pushback(dead, (int)thread);
  101bc0:	83 ec 08             	sub    esp,0x8
  101bc3:	53                   	push   ebx
  101bc4:	ff 35 20 80 11 00    	push   DWORD PTR ds:0x118020
  101bca:	e8 b1 65 00 00       	call   108180 <list_pushback>
  101bcf:	83 c4 10             	add    esp,0x10
  101bd2:	eb d5                	jmp    101ba9 <thread_cancel+0x29>
  101bd4:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
  101bda:	8d bf 00 00 00 00    	lea    edi,[edi+0x0]

00101be0 <thread_exit>:
{
  101be0:	53                   	push   ebx
  101be1:	83 ec 08             	sub    esp,0x8
    return running;
  101be4:	8b 1d 1c 80 11 00    	mov    ebx,DWORD PTR ds:0x11801c
    atomic_begin();
  101bea:	e8 51 62 00 00       	call   107e40 <atomic_begin>
    if (thread == NULL)
  101bef:	85 db                	test   ebx,ebx
  101bf1:	74 1b                	je     101c0e <thread_exit+0x2e>
    if (!list_containe(dead, (int)thread))
  101bf3:	83 ec 08             	sub    esp,0x8
  101bf6:	53                   	push   ebx
  101bf7:	ff 35 20 80 11 00    	push   DWORD PTR ds:0x118020
  101bfd:	e8 ce 66 00 00       	call   1082d0 <list_containe>
  101c02:	83 c4 10             	add    esp,0x10
  101c05:	85 c0                	test   eax,eax
  101c07:	74 0a                	je     101c13 <thread_exit+0x33>
    atomic_end();
  101c09:	e8 52 62 00 00       	call   107e60 <atomic_end>
    asm("int $32"); // yield
  101c0e:	cd 20                	int    0x20
        hlt();
  101c10:	f4                   	hlt    
  101c11:	eb fd                	jmp    101c10 <thread_exit+0x30>
        list_pushback(dead, (int)thread);
  101c13:	50                   	push   eax
  101c14:	50                   	push   eax
  101c15:	53                   	push   ebx
  101c16:	ff 35 20 80 11 00    	push   DWORD PTR ds:0x118020
  101c1c:	e8 5f 65 00 00       	call   108180 <list_pushback>
  101c21:	83 c4 10             	add    esp,0x10
  101c24:	eb e3                	jmp    101c09 <thread_exit+0x29>
  101c26:	8d 76 00             	lea    esi,[esi+0x0]
  101c29:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00101c30 <thread_self>:
}
  101c30:	a1 1c 80 11 00       	mov    eax,ds:0x11801c
  101c35:	c3                   	ret    
  101c36:	8d 76 00             	lea    esi,[esi+0x0]
  101c39:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00101c40 <load_elfseg>:
{
  101c40:	55                   	push   ebp
  101c41:	57                   	push   edi
  101c42:	56                   	push   esi
  101c43:	53                   	push   ebx
  101c44:	83 ec 1c             	sub    esp,0x1c
  101c47:	8b 44 24 34          	mov    eax,DWORD PTR [esp+0x34]
  101c4b:	8b 5c 24 3c          	mov    ebx,DWORD PTR [esp+0x3c]
  101c4f:	8b 74 24 40          	mov    esi,DWORD PTR [esp+0x40]
  101c53:	8b 7c 24 38          	mov    edi,DWORD PTR [esp+0x38]
  101c57:	8b 6c 24 30          	mov    ebp,DWORD PTR [esp+0x30]
  101c5b:	89 44 24 0c          	mov    DWORD PTR [esp+0xc],eax
    atomic_begin();
  101c5f:	e8 dc 61 00 00       	call   107e40 <atomic_begin>
    log("Loading ELF segment: SRC=0x%x(%d) DEST=0x%x(%d)", src, srcsz, dest, destsz);
  101c64:	83 ec 0c             	sub    esp,0xc
  101c67:	56                   	push   esi
  101c68:	53                   	push   ebx
  101c69:	57                   	push   edi
  101c6a:	ff 74 24 24          	push   DWORD PTR [esp+0x24]
  101c6e:	68 f8 62 11 00       	push   0x1162f8
  101c73:	e8 28 fa ff ff       	call   1016a0 <log>
    paging_load_directorie(process->pdir);
  101c78:	83 c4 14             	add    esp,0x14
  101c7b:	ff b5 8c 00 00 00    	push   DWORD PTR [ebp+0x8c]
  101c81:	e8 96 37 00 00       	call   10541c <paging_load_directorie>
    paging_invalidate_tlb();
  101c86:	e8 99 37 00 00       	call   105424 <paging_invalidate_tlb>
    log("ok");
  101c8b:	c7 04 24 8b 1f 11 00 	mov    DWORD PTR [esp],0x111f8b
  101c92:	e8 09 fa ff ff       	call   1016a0 <log>
    process_map(process, dest, PAGE_ALIGN(destsz) / PAGE_SIZE);
  101c97:	89 f0                	mov    eax,esi
    return memory_map(process->pdir, addr, count, 1);
  101c99:	6a 01                	push   0x1
    process_map(process, dest, PAGE_ALIGN(destsz) / PAGE_SIZE);
  101c9b:	25 00 f0 ff ff       	and    eax,0xfffff000
  101ca0:	05 00 10 00 00       	add    eax,0x1000
  101ca5:	c1 e8 0c             	shr    eax,0xc
    return memory_map(process->pdir, addr, count, 1);
  101ca8:	50                   	push   eax
  101ca9:	53                   	push   ebx
  101caa:	ff b5 8c 00 00 00    	push   DWORD PTR [ebp+0x8c]
  101cb0:	e8 db 14 00 00       	call   103190 <memory_map>
    log("ok");
  101cb5:	83 c4 14             	add    esp,0x14
  101cb8:	68 8b 1f 11 00       	push   0x111f8b
  101cbd:	e8 de f9 ff ff       	call   1016a0 <log>
    memset((void *)dest, 0, destsz);
  101cc2:	83 c4 0c             	add    esp,0xc
  101cc5:	56                   	push   esi
  101cc6:	6a 00                	push   0x0
  101cc8:	53                   	push   ebx
  101cc9:	e8 02 3a 00 00       	call   1056d0 <memset>
    log("ok");
  101cce:	c7 04 24 8b 1f 11 00 	mov    DWORD PTR [esp],0x111f8b
  101cd5:	e8 c6 f9 ff ff       	call   1016a0 <log>
    memcpy((void *)dest, (void *)src, srcsz);
  101cda:	83 c4 0c             	add    esp,0xc
  101cdd:	57                   	push   edi
  101cde:	ff 74 24 14          	push   DWORD PTR [esp+0x14]
  101ce2:	53                   	push   ebx
  101ce3:	e8 28 39 00 00       	call   105610 <memcpy>
    log("switching page directorie");
  101ce8:	c7 04 24 8e 1f 11 00 	mov    DWORD PTR [esp],0x111f8e
  101cef:	e8 ac f9 ff ff       	call   1016a0 <log>
    paging_load_directorie(memory_kpdir());
  101cf4:	e8 87 0f 00 00       	call   102c80 <memory_kpdir>
  101cf9:	89 04 24             	mov    DWORD PTR [esp],eax
  101cfc:	e8 1b 37 00 00       	call   10541c <paging_load_directorie>
}
  101d01:	83 c4 2c             	add    esp,0x2c
  101d04:	5b                   	pop    ebx
  101d05:	5e                   	pop    esi
  101d06:	5f                   	pop    edi
  101d07:	5d                   	pop    ebp
    atomic_end();
  101d08:	e9 53 61 00 00       	jmp    107e60 <atomic_end>
  101d0d:	8d 76 00             	lea    esi,[esi+0x0]

00101d10 <process_exec>:
{
  101d10:	55                   	push   ebp
  101d11:	57                   	push   edi
  101d12:	56                   	push   esi
  101d13:	53                   	push   ebx
  101d14:	83 ec 44             	sub    esp,0x44
  101d17:	8b 74 24 58          	mov    esi,DWORD PTR [esp+0x58]
    file_t *fp = file_open(NULL, path);
  101d1b:	56                   	push   esi
  101d1c:	6a 00                	push   0x0
  101d1e:	e8 ed f3 ff ff       	call   101110 <file_open>
    if (!fp)
  101d23:	83 c4 10             	add    esp,0x10
  101d26:	85 c0                	test   eax,eax
  101d28:	0f 84 ce 01 00 00    	je     101efc <process_exec+0x1ec>
    process_t *process = (process_t *)malloc(sizeof(process_t));
  101d2e:	83 ec 0c             	sub    esp,0xc
  101d31:	89 c3                	mov    ebx,eax
  101d33:	68 90 00 00 00       	push   0x90
  101d38:	e8 63 52 00 00       	call   106fa0 <malloc>
  101d3d:	89 c5                	mov    ebp,eax
    process->id = PID++;
  101d3f:	a1 08 ae 23 00       	mov    eax,ds:0x23ae08
    strncpy(process->name, name, PROCNAME_SIZE);
  101d44:	83 c4 0c             	add    esp,0xc
    process->id = PID++;
  101d47:	8d 50 01             	lea    edx,[eax+0x1]
  101d4a:	89 15 08 ae 23 00    	mov    DWORD PTR ds:0x23ae08,edx
  101d50:	89 45 00             	mov    DWORD PTR [ebp+0x0],eax
    strncpy(process->name, name, PROCNAME_SIZE);
  101d53:	8d 45 04             	lea    eax,[ebp+0x4]
  101d56:	68 80 00 00 00       	push   0x80
  101d5b:	56                   	push   esi
  101d5c:	50                   	push   eax
  101d5d:	e8 de 40 00 00       	call   105e40 <strncpy>
    process->user = user;
  101d62:	c6 85 84 00 00 00 01 	mov    BYTE PTR [ebp+0x84],0x1
    process->threads = list_alloc();
  101d69:	e8 52 61 00 00       	call   107ec0 <list_alloc>
  101d6e:	89 85 88 00 00 00    	mov    DWORD PTR [ebp+0x88],eax
        process->pdir = memory_alloc_pdir();
  101d74:	e8 27 11 00 00       	call   102ea0 <memory_alloc_pdir>
  101d79:	89 85 8c 00 00 00    	mov    DWORD PTR [ebp+0x8c],eax
    void *buffer = file_read_all(fp);
  101d7f:	89 1c 24             	mov    DWORD PTR [esp],ebx
  101d82:	e8 69 f5 ff ff       	call   1012f0 <file_read_all>
  101d87:	89 c7                	mov    edi,eax
    file_close(fp);
  101d89:	89 1c 24             	mov    DWORD PTR [esp],ebx
  101d8c:	e8 ff f4 ff ff       	call   101290 <file_close>
    log("ELF file: VALID=%d TYPE=%d ENTRY=0x%x SEG_COUNT=%i", ELF_valid(elf), elf->type, elf->entry, elf->phnum);
  101d91:	0f b7 57 2c          	movzx  edx,WORD PTR [edi+0x2c]
  101d95:	0f b7 5f 10          	movzx  ebx,WORD PTR [edi+0x10]
  101d99:	8b 77 18             	mov    esi,DWORD PTR [edi+0x18]
  101d9c:	89 3c 24             	mov    DWORD PTR [esp],edi
  101d9f:	89 54 24 14          	mov    DWORD PTR [esp+0x14],edx
  101da3:	e8 68 69 00 00       	call   108710 <ELF_valid>
  101da8:	8b 54 24 14          	mov    edx,DWORD PTR [esp+0x14]
  101dac:	89 14 24             	mov    DWORD PTR [esp],edx
  101daf:	56                   	push   esi
  101db0:	53                   	push   ebx
  101db1:	50                   	push   eax
  101db2:	68 50 63 11 00       	push   0x116350
  101db7:	e8 e4 f8 ff ff       	call   1016a0 <log>
    for (int i = 0; ELF_read_program(elf, &program, i); i++)
  101dbc:	83 c4 20             	add    esp,0x20
  101dbf:	c7 44 24 08 00 00 00 	mov    DWORD PTR [esp+0x8],0x0
  101dc6:	00 
  101dc7:	89 7c 24 04          	mov    DWORD PTR [esp+0x4],edi
  101dcb:	89 6c 24 0c          	mov    DWORD PTR [esp+0xc],ebp
  101dcf:	e9 d8 00 00 00       	jmp    101eac <process_exec+0x19c>
  101dd4:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
        printf("\n");
  101dd8:	83 ec 0c             	sub    esp,0xc
  101ddb:	68 f9 1f 11 00       	push   0x111ff9
  101de0:	e8 8b 48 00 00       	call   106670 <printf>
        load_elfseg(process, (uint)(buffer) + program.offset, program.filesz, program.vaddr, program.memsz);
  101de5:	8b 7c 24 14          	mov    edi,DWORD PTR [esp+0x14]
  101de9:	03 7c 24 24          	add    edi,DWORD PTR [esp+0x24]
  101ded:	8b 74 24 34          	mov    esi,DWORD PTR [esp+0x34]
  101df1:	8b 5c 24 28          	mov    ebx,DWORD PTR [esp+0x28]
  101df5:	8b 6c 24 30          	mov    ebp,DWORD PTR [esp+0x30]
    atomic_begin();
  101df9:	e8 42 60 00 00       	call   107e40 <atomic_begin>
    log("Loading ELF segment: SRC=0x%x(%d) DEST=0x%x(%d)", src, srcsz, dest, destsz);
  101dfe:	89 34 24             	mov    DWORD PTR [esp],esi
  101e01:	53                   	push   ebx
  101e02:	55                   	push   ebp
  101e03:	57                   	push   edi
  101e04:	68 f8 62 11 00       	push   0x1162f8
  101e09:	e8 92 f8 ff ff       	call   1016a0 <log>
    paging_load_directorie(process->pdir);
  101e0e:	83 c4 14             	add    esp,0x14
  101e11:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
  101e15:	ff b0 8c 00 00 00    	push   DWORD PTR [eax+0x8c]
  101e1b:	e8 fc 35 00 00       	call   10541c <paging_load_directorie>
    paging_invalidate_tlb();
  101e20:	e8 ff 35 00 00       	call   105424 <paging_invalidate_tlb>
    log("ok");
  101e25:	c7 04 24 8b 1f 11 00 	mov    DWORD PTR [esp],0x111f8b
  101e2c:	e8 6f f8 ff ff       	call   1016a0 <log>
    process_map(process, dest, PAGE_ALIGN(destsz) / PAGE_SIZE);
  101e31:	89 f1                	mov    ecx,esi
    return memory_map(process->pdir, addr, count, 1);
  101e33:	6a 01                	push   0x1
    process_map(process, dest, PAGE_ALIGN(destsz) / PAGE_SIZE);
  101e35:	81 e1 00 f0 ff ff    	and    ecx,0xfffff000
  101e3b:	89 c8                	mov    eax,ecx
  101e3d:	05 00 10 00 00       	add    eax,0x1000
  101e42:	c1 e8 0c             	shr    eax,0xc
    return memory_map(process->pdir, addr, count, 1);
  101e45:	50                   	push   eax
  101e46:	53                   	push   ebx
  101e47:	8b 44 24 28          	mov    eax,DWORD PTR [esp+0x28]
  101e4b:	ff b0 8c 00 00 00    	push   DWORD PTR [eax+0x8c]
  101e51:	e8 3a 13 00 00       	call   103190 <memory_map>
    log("ok");
  101e56:	83 c4 14             	add    esp,0x14
  101e59:	68 8b 1f 11 00       	push   0x111f8b
  101e5e:	e8 3d f8 ff ff       	call   1016a0 <log>
    memset((void *)dest, 0, destsz);
  101e63:	83 c4 0c             	add    esp,0xc
  101e66:	56                   	push   esi
  101e67:	6a 00                	push   0x0
  101e69:	53                   	push   ebx
  101e6a:	e8 61 38 00 00       	call   1056d0 <memset>
    log("ok");
  101e6f:	c7 04 24 8b 1f 11 00 	mov    DWORD PTR [esp],0x111f8b
  101e76:	e8 25 f8 ff ff       	call   1016a0 <log>
    memcpy((void *)dest, (void *)src, srcsz);
  101e7b:	83 c4 0c             	add    esp,0xc
  101e7e:	55                   	push   ebp
  101e7f:	57                   	push   edi
  101e80:	53                   	push   ebx
  101e81:	e8 8a 37 00 00       	call   105610 <memcpy>
    log("switching page directorie");
  101e86:	c7 04 24 8e 1f 11 00 	mov    DWORD PTR [esp],0x111f8e
  101e8d:	e8 0e f8 ff ff       	call   1016a0 <log>
    paging_load_directorie(memory_kpdir());
  101e92:	e8 e9 0d 00 00       	call   102c80 <memory_kpdir>
  101e97:	89 04 24             	mov    DWORD PTR [esp],eax
  101e9a:	e8 7d 35 00 00       	call   10541c <paging_load_directorie>
    atomic_end();
  101e9f:	e8 bc 5f 00 00       	call   107e60 <atomic_end>
    for (int i = 0; ELF_read_program(elf, &program, i); i++)
  101ea4:	83 44 24 18 01       	add    DWORD PTR [esp+0x18],0x1
  101ea9:	83 c4 10             	add    esp,0x10
  101eac:	83 ec 04             	sub    esp,0x4
  101eaf:	ff 74 24 0c          	push   DWORD PTR [esp+0xc]
  101eb3:	8d 44 24 18          	lea    eax,[esp+0x18]
  101eb7:	50                   	push   eax
  101eb8:	ff 74 24 10          	push   DWORD PTR [esp+0x10]
  101ebc:	e8 1f 69 00 00       	call   1087e0 <ELF_read_program>
  101ec1:	83 c4 10             	add    esp,0x10
  101ec4:	85 c0                	test   eax,eax
  101ec6:	0f 85 0c ff ff ff    	jne    101dd8 <process_exec+0xc8>
  101ecc:	8b 7c 24 04          	mov    edi,DWORD PTR [esp+0x4]
  101ed0:	8b 6c 24 0c          	mov    ebp,DWORD PTR [esp+0xc]
    paging_load_directorie(process->pdir);
  101ed4:	83 ec 0c             	sub    esp,0xc
    thread_entry_t entry = (thread_entry_t)elf->entry;
  101ed7:	8b 5f 18             	mov    ebx,DWORD PTR [edi+0x18]
    paging_load_directorie(process->pdir);
  101eda:	ff b5 8c 00 00 00    	push   DWORD PTR [ebp+0x8c]
  101ee0:	e8 37 35 00 00       	call   10541c <paging_load_directorie>
    entry();
  101ee5:	ff d3                	call   ebx
    free(buffer);
  101ee7:	89 3c 24             	mov    DWORD PTR [esp],edi
  101eea:	e8 81 55 00 00       	call   107470 <free>
    return process;
  101eef:	83 c4 10             	add    esp,0x10
}
  101ef2:	83 c4 3c             	add    esp,0x3c
  101ef5:	89 e8                	mov    eax,ebp
  101ef7:	5b                   	pop    ebx
  101ef8:	5e                   	pop    esi
  101ef9:	5f                   	pop    edi
  101efa:	5d                   	pop    ebp
  101efb:	c3                   	ret    
        log("EXEC: %s file not found, exec failed!", path);
  101efc:	83 ec 08             	sub    esp,0x8
        return NULL;
  101eff:	31 ed                	xor    ebp,ebp
        log("EXEC: %s file not found, exec failed!", path);
  101f01:	56                   	push   esi
  101f02:	68 28 63 11 00       	push   0x116328
  101f07:	e8 94 f7 ff ff       	call   1016a0 <log>
        return NULL;
  101f0c:	83 c4 10             	add    esp,0x10
  101f0f:	eb e1                	jmp    101ef2 <process_exec+0x1e2>
  101f11:	eb 0d                	jmp    101f20 <process_kill>
  101f13:	90                   	nop
  101f14:	90                   	nop
  101f15:	90                   	nop
  101f16:	90                   	nop
  101f17:	90                   	nop
  101f18:	90                   	nop
  101f19:	90                   	nop
  101f1a:	90                   	nop
  101f1b:	90                   	nop
  101f1c:	90                   	nop
  101f1d:	90                   	nop
  101f1e:	90                   	nop
  101f1f:	90                   	nop

00101f20 <process_kill>:
{
  101f20:	53                   	push   ebx
  101f21:	83 ec 08             	sub    esp,0x8
    atomic_begin();
  101f24:	e8 17 5f 00 00       	call   107e40 <atomic_begin>
    FOREACH(i, process->threads)
  101f29:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  101f2d:	8b 80 88 00 00 00    	mov    eax,DWORD PTR [eax+0x88]
  101f33:	8b 58 04             	mov    ebx,DWORD PTR [eax+0x4]
  101f36:	85 db                	test   ebx,ebx
  101f38:	74 2d                	je     101f67 <process_kill+0x47>
  101f3a:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
    atomic_begin();
  101f40:	e8 fb 5e 00 00       	call   107e40 <atomic_begin>
    if (!list_containe(dead, (int)thread))
  101f45:	83 ec 08             	sub    esp,0x8
  101f48:	53                   	push   ebx
  101f49:	ff 35 20 80 11 00    	push   DWORD PTR ds:0x118020
  101f4f:	e8 7c 63 00 00       	call   1082d0 <list_containe>
  101f54:	83 c4 10             	add    esp,0x10
  101f57:	85 c0                	test   eax,eax
  101f59:	74 1d                	je     101f78 <process_kill+0x58>
    atomic_end();
  101f5b:	e8 00 5f 00 00       	call   107e60 <atomic_end>
    FOREACH(i, process->threads)
  101f60:	8b 5b 04             	mov    ebx,DWORD PTR [ebx+0x4]
  101f63:	85 db                	test   ebx,ebx
  101f65:	75 d9                	jne    101f40 <process_kill+0x20>
    atomic_end();
  101f67:	e8 f4 5e 00 00       	call   107e60 <atomic_end>
    asm("int $32"); // yield
  101f6c:	cd 20                	int    0x20
}
  101f6e:	83 c4 08             	add    esp,0x8
  101f71:	5b                   	pop    ebx
  101f72:	c3                   	ret    
  101f73:	90                   	nop
  101f74:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
        list_pushback(dead, (int)thread);
  101f78:	83 ec 08             	sub    esp,0x8
  101f7b:	53                   	push   ebx
  101f7c:	ff 35 20 80 11 00    	push   DWORD PTR ds:0x118020
  101f82:	e8 f9 61 00 00       	call   108180 <list_pushback>
    atomic_end();
  101f87:	e8 d4 5e 00 00       	call   107e60 <atomic_end>
    FOREACH(i, process->threads)
  101f8c:	8b 5b 04             	mov    ebx,DWORD PTR [ebx+0x4]
  101f8f:	83 c4 10             	add    esp,0x10
  101f92:	85 db                	test   ebx,ebx
  101f94:	75 aa                	jne    101f40 <process_kill+0x20>
  101f96:	eb cf                	jmp    101f67 <process_kill+0x47>
  101f98:	90                   	nop
  101f99:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]

00101fa0 <process_self>:
    return running->process;
  101fa0:	a1 1c 80 11 00       	mov    eax,ds:0x11801c
  101fa5:	8b 40 10             	mov    eax,DWORD PTR [eax+0x10]
}
  101fa8:	c3                   	ret    
  101fa9:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]

00101fb0 <process_exit>:
{
  101fb0:	53                   	push   ebx
  101fb1:	83 ec 08             	sub    esp,0x8
    return running->process;
  101fb4:	a1 1c 80 11 00       	mov    eax,ds:0x11801c
  101fb9:	8b 58 10             	mov    ebx,DWORD PTR [eax+0x10]
    if (self != kernel_process)
  101fbc:	39 1d 28 80 11 00    	cmp    DWORD PTR ds:0x118028,ebx
  101fc2:	74 7c                	je     102040 <process_exit+0x90>
    atomic_begin();
  101fc4:	e8 77 5e 00 00       	call   107e40 <atomic_begin>
    FOREACH(i, process->threads)
  101fc9:	8b 83 88 00 00 00    	mov    eax,DWORD PTR [ebx+0x88]
  101fcf:	8b 58 04             	mov    ebx,DWORD PTR [eax+0x4]
  101fd2:	85 db                	test   ebx,ebx
  101fd4:	74 31                	je     102007 <process_exit+0x57>
  101fd6:	8d 76 00             	lea    esi,[esi+0x0]
  101fd9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
    atomic_begin();
  101fe0:	e8 5b 5e 00 00       	call   107e40 <atomic_begin>
    if (!list_containe(dead, (int)thread))
  101fe5:	83 ec 08             	sub    esp,0x8
  101fe8:	53                   	push   ebx
  101fe9:	ff 35 20 80 11 00    	push   DWORD PTR ds:0x118020
  101fef:	e8 dc 62 00 00       	call   1082d0 <list_containe>
  101ff4:	83 c4 10             	add    esp,0x10
  101ff7:	85 c0                	test   eax,eax
  101ff9:	74 1d                	je     102018 <process_exit+0x68>
    atomic_end();
  101ffb:	e8 60 5e 00 00       	call   107e60 <atomic_end>
    FOREACH(i, process->threads)
  102000:	8b 5b 04             	mov    ebx,DWORD PTR [ebx+0x4]
  102003:	85 db                	test   ebx,ebx
  102005:	75 d9                	jne    101fe0 <process_exit+0x30>
    atomic_end();
  102007:	e8 54 5e 00 00       	call   107e60 <atomic_end>
    asm("int $32"); // yield
  10200c:	cd 20                	int    0x20
}
  10200e:	83 c4 08             	add    esp,0x8
  102011:	5b                   	pop    ebx
  102012:	c3                   	ret    
  102013:	90                   	nop
  102014:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
        list_pushback(dead, (int)thread);
  102018:	83 ec 08             	sub    esp,0x8
  10201b:	53                   	push   ebx
  10201c:	ff 35 20 80 11 00    	push   DWORD PTR ds:0x118020
  102022:	e8 59 61 00 00       	call   108180 <list_pushback>
    atomic_end();
  102027:	e8 34 5e 00 00       	call   107e60 <atomic_end>
    FOREACH(i, process->threads)
  10202c:	8b 5b 04             	mov    ebx,DWORD PTR [ebx+0x4]
  10202f:	83 c4 10             	add    esp,0x10
  102032:	85 db                	test   ebx,ebx
  102034:	75 aa                	jne    101fe0 <process_exit+0x30>
  102036:	eb cf                	jmp    102007 <process_exit+0x57>
  102038:	90                   	nop
  102039:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
        PANIC("Kernel commit suicide!");
  102040:	83 ec 0c             	sub    esp,0xc
  102043:	68 a8 1f 11 00       	push   0x111fa8
  102048:	6a 00                	push   0x0
  10204a:	68 17 01 00 00       	push   0x117
  10204f:	68 34 1d 11 00       	push   0x111d34
  102054:	68 84 63 11 00       	push   0x116384
  102059:	e8 52 00 00 00       	call   1020b0 <__panic>
  10205e:	83 c4 20             	add    esp,0x20
}
  102061:	83 c4 08             	add    esp,0x8
  102064:	5b                   	pop    ebx
  102065:	c3                   	ret    
  102066:	8d 76 00             	lea    esi,[esi+0x0]
  102069:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00102070 <process_map>:
{
  102070:	83 ec 0c             	sub    esp,0xc
    return memory_map(process->pdir, addr, count, 1);
  102073:	6a 01                	push   0x1
  102075:	ff 74 24 1c          	push   DWORD PTR [esp+0x1c]
  102079:	ff 74 24 1c          	push   DWORD PTR [esp+0x1c]
  10207d:	8b 44 24 1c          	mov    eax,DWORD PTR [esp+0x1c]
  102081:	ff b0 8c 00 00 00    	push   DWORD PTR [eax+0x8c]
  102087:	e8 04 11 00 00       	call   103190 <memory_map>
}
  10208c:	83 c4 1c             	add    esp,0x1c
  10208f:	c3                   	ret    

00102090 <process_unmap>:
    return memory_unmap(process->pdir, addr, count);
  102090:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  102094:	8b 80 8c 00 00 00    	mov    eax,DWORD PTR [eax+0x8c]
  10209a:	89 44 24 04          	mov    DWORD PTR [esp+0x4],eax
  10209e:	e9 9d 12 00 00       	jmp    103340 <memory_unmap>
  1020a3:	66 90                	xchg   ax,ax
  1020a5:	66 90                	xchg   ax,ax
  1020a7:	66 90                	xchg   ax,ax
  1020a9:	66 90                	xchg   ax,ax
  1020ab:	66 90                	xchg   ax,ax
  1020ad:	66 90                	xchg   ax,ax
  1020af:	90                   	nop

001020b0 <__panic>:
    "System consumed all the paper for paging!",
    "Suspicious pointer corrupted the machine."
};

void __panic(const string file, const string function, const int line, context_t *context, string message, ...)
{
  1020b0:	53                   	push   ebx
  1020b1:	83 ec 14             	sub    esp,0x14
  1020b4:	8b 5c 24 28          	mov    ebx,DWORD PTR [esp+0x28]
    cli();
  1020b8:	fa                   	cli    
    va_list va;
    va_start(va, message);

    printf("\n--- !!! ------------------------------------------------------------------------\n");
  1020b9:	68 a8 63 11 00       	push   0x1163a8
  1020be:	e8 ad 45 00 00       	call   106670 <printf>

    printf("\n\tKERNEL PANIC\n\t// %s\n\n\t", witty_comments[timer_get_ticks() % (9 + 4)]);
  1020c3:	e8 58 1b 00 00       	call   103c20 <timer_get_ticks>
  1020c8:	ba 4f ec c4 4e       	mov    edx,0x4ec4ec4f
  1020cd:	89 c1                	mov    ecx,eax
  1020cf:	83 c4 08             	add    esp,0x8
  1020d2:	f7 e2                	mul    edx
  1020d4:	89 d0                	mov    eax,edx
  1020d6:	c1 e8 02             	shr    eax,0x2
  1020d9:	8d 14 40             	lea    edx,[eax+eax*2]
  1020dc:	8d 04 90             	lea    eax,[eax+edx*4]
  1020df:	29 c1                	sub    ecx,eax
  1020e1:	ff 34 8d 60 1d 11 00 	push   DWORD PTR [ecx*4+0x111d60]
  1020e8:	68 bf 1f 11 00       	push   0x111fbf
  1020ed:	e8 7e 45 00 00       	call   106670 <printf>

    vprintf(message, va);
  1020f2:	58                   	pop    eax
  1020f3:	5a                   	pop    edx
  1020f4:	8d 44 24 2c          	lea    eax,[esp+0x2c]
  1020f8:	50                   	push   eax
  1020f9:	ff 74 24 2c          	push   DWORD PTR [esp+0x2c]
  1020fd:	e8 9e 47 00 00       	call   1068a0 <vprintf>
    printf("\n\tat %s %s() ln%d", file, function, line);
  102102:	ff 74 24 28          	push   DWORD PTR [esp+0x28]
  102106:	ff 74 24 28          	push   DWORD PTR [esp+0x28]
  10210a:	ff 74 24 28          	push   DWORD PTR [esp+0x28]
  10210e:	68 d8 1f 11 00       	push   0x111fd8
  102113:	e8 58 45 00 00       	call   106670 <printf>

    printf("\n");
  102118:	83 c4 14             	add    esp,0x14
  10211b:	68 f9 1f 11 00       	push   0x111ff9
  102120:	e8 4b 45 00 00       	call   106670 <printf>
    printf("\n\tDiagnostic:");
  102125:	c7 04 24 ea 1f 11 00 	mov    DWORD PTR [esp],0x111fea
  10212c:	e8 3f 45 00 00       	call   106670 <printf>
    printf("\n\tThe system was running for %d tick.", timer_get_ticks());
  102131:	e8 ea 1a 00 00       	call   103c20 <timer_get_ticks>
  102136:	59                   	pop    ecx
  102137:	5a                   	pop    edx
  102138:	50                   	push   eax
  102139:	68 fc 63 11 00       	push   0x1163fc
  10213e:	e8 2d 45 00 00       	call   106670 <printf>
    printf("\n\n");
  102143:	c7 04 24 f8 1f 11 00 	mov    DWORD PTR [esp],0x111ff8
  10214a:	e8 21 45 00 00       	call   106670 <printf>

    if (context != NULL)
  10214f:	83 c4 10             	add    esp,0x10
  102152:	85 db                	test   ebx,ebx
  102154:	74 0c                	je     102162 <__panic+0xb2>
    {
        dump_context(context);
  102156:	83 ec 0c             	sub    esp,0xc
  102159:	53                   	push   ebx
  10215a:	e8 21 23 00 00       	call   104480 <dump_context>
  10215f:	83 c4 10             	add    esp,0x10
    }

    puts("\n\tSystem halted!\n");
  102162:	83 ec 0c             	sub    esp,0xc
  102165:	68 fb 1f 11 00       	push   0x111ffb
  10216a:	e8 31 57 00 00       	call   1078a0 <puts>
  10216f:	83 c4 10             	add    esp,0x10
  102172:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]

    STOP;
  102178:	fa                   	cli    
  102179:	f4                   	hlt    
  10217a:	eb fc                	jmp    102178 <__panic+0xc8>
  10217c:	66 90                	xchg   ax,ax
  10217e:	66 90                	xchg   ax,ax

00102180 <virtual_map.constprop.2>:
    page_t *p = &ptable->pages[pti];

    return ((p->PageFrameNumber & ~0xfff) + (vaddr & 0xfff));
}

int virtual_map(page_directorie_t *pdir, uint vaddr, uint paddr, uint count, bool user)
  102180:	55                   	push   ebp
  102181:	57                   	push   edi
  102182:	89 c7                	mov    edi,eax
  102184:	56                   	push   esi
  102185:	53                   	push   ebx
  102186:	89 d6                	mov    esi,edx
  102188:	83 ec 2c             	sub    esp,0x2c
  10218b:	8b 5c 24 40          	mov    ebx,DWORD PTR [esp+0x40]
  10218f:	89 4c 24 18          	mov    DWORD PTR [esp+0x18],ecx
{

    log("VMAP VADDR=0x%x PADDR=0x%x COUNT=%d", vaddr, paddr, count);
  102193:	6a 01                	push   0x1
  102195:	51                   	push   ecx
  102196:	52                   	push   edx
  102197:	68 b4 65 11 00       	push   0x1165b4
  10219c:	e8 ff f4 ff ff       	call   1016a0 <log>
    for (uint i = 0; i < count; i++)
    {
        uint offset = i * PAGE_SIZE;

        uint pdi = PD_INDEX(vaddr + offset);
  1021a1:	89 f1                	mov    ecx,esi
  1021a3:	89 d8                	mov    eax,ebx
        uint pti = PT_INDEX(vaddr + offset);
  1021a5:	c1 ee 0c             	shr    esi,0xc
        uint pdi = PD_INDEX(vaddr + offset);
  1021a8:	c1 e9 16             	shr    ecx,0x16
  1021ab:	83 e0 01             	and    eax,0x1
        uint pti = PT_INDEX(vaddr + offset);
  1021ae:	81 e6 ff 03 00 00    	and    esi,0x3ff
        uint pdi = PD_INDEX(vaddr + offset);
  1021b4:	89 4c 24 2c          	mov    DWORD PTR [esp+0x2c],ecx
  1021b8:	88 44 24 27          	mov    BYTE PTR [esp+0x27],al

        page_directorie_entry_t *pde = &pdir->entries[pdi];
        page_table_t *ptable = (page_table_t *)(pde->PageFrameNumber * PAGE_SIZE);

        if (!pde->Present)
  1021bc:	83 c4 10             	add    esp,0x10
  1021bf:	f6 04 8f 01          	test   BYTE PTR [edi+ecx*4],0x1
  1021c3:	74 6b                	je     102230 <virtual_map.constprop.2+0xb0>
        page_table_t *ptable = (page_table_t *)(pde->PageFrameNumber * PAGE_SIZE);
  1021c5:	0f b6 44 8f 01       	movzx  eax,BYTE PTR [edi+ecx*4+0x1]
  1021ca:	88 44 24 0c          	mov    BYTE PTR [esp+0xc],al
  1021ce:	0f b6 5c 8f 02       	movzx  ebx,BYTE PTR [edi+ecx*4+0x2]
  1021d3:	c0 e8 04             	shr    al,0x4
  1021d6:	0f b6 c0             	movzx  eax,al
  1021d9:	c1 e3 04             	shl    ebx,0x4
  1021dc:	09 d8                	or     eax,ebx
  1021de:	0f b6 5c 8f 03       	movzx  ebx,BYTE PTR [edi+ecx*4+0x3]
  1021e3:	c1 e3 0c             	shl    ebx,0xc
  1021e6:	09 c3                	or     ebx,eax
  1021e8:	c1 e3 0c             	shl    ebx,0xc
            pde->User = user;
            pde->PageFrameNumber = (u32)(ptable) >> 12;
        }

        page_t *p = &ptable->pages[pti];
        p->Present = 1;
  1021eb:	0f b6 04 b3          	movzx  eax,BYTE PTR [ebx+esi*4]
        p->User = user;
  1021ef:	0f b6 54 24 17       	movzx  edx,BYTE PTR [esp+0x17]
        p->Present = 1;
  1021f4:	83 c8 01             	or     eax,0x1
        p->User = user;
  1021f7:	c1 e2 02             	shl    edx,0x2
  1021fa:	83 e0 fb             	and    eax,0xfffffffb
  1021fd:	09 d0                	or     eax,edx
        p->Write = 1;
        p->PageFrameNumber = (paddr + offset) >> 12;
  1021ff:	8b 54 24 18          	mov    edx,DWORD PTR [esp+0x18]
        p->Write = 1;
  102203:	83 c8 02             	or     eax,0x2
  102206:	88 04 b3             	mov    BYTE PTR [ebx+esi*4],al
        p->PageFrameNumber = (paddr + offset) >> 12;
  102209:	8b 04 b3             	mov    eax,DWORD PTR [ebx+esi*4]
  10220c:	81 e2 00 f0 ff ff    	and    edx,0xfffff000
  102212:	25 ff 0f 00 00       	and    eax,0xfff
  102217:	09 d0                	or     eax,edx
  102219:	89 04 b3             	mov    DWORD PTR [ebx+esi*4],eax
    }

    paging_invalidate_tlb();
  10221c:	e8 03 32 00 00       	call   105424 <paging_invalidate_tlb>

    return 0;
}
  102221:	83 c4 2c             	add    esp,0x2c
  102224:	31 c0                	xor    eax,eax
  102226:	5b                   	pop    ebx
  102227:	5e                   	pop    esi
  102228:	5f                   	pop    edi
  102229:	5d                   	pop    ebp
  10222a:	c3                   	ret    
  10222b:	90                   	nop
  10222c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
            log("Missing page table! Allocating a new one");
  102230:	83 ec 0c             	sub    esp,0xc
  102233:	68 d8 65 11 00       	push   0x1165d8
  102238:	e8 63 f4 ff ff       	call   1016a0 <log>
    return &kpdir;
}

uint memory_alloc(uint count)
{
    atomic_begin();
  10223d:	e8 fe 5b 00 00       	call   107e40 <atomic_begin>
    log("PALLOC COUNT=%d", count);
  102242:	58                   	pop    eax
  102243:	5a                   	pop    edx
  102244:	6a 01                	push   0x1
  102246:	68 82 20 11 00       	push   0x112082
  10224b:	e8 50 f4 ff ff       	call   1016a0 <log>
    for (uint i = 0; i < (TOTAL_MEMORY / PAGE_SIZE); i++)
  102250:	a1 10 ae 23 00       	mov    eax,ds:0x23ae10
  102255:	83 c4 10             	add    esp,0x10
  102258:	c1 e8 0c             	shr    eax,0xc
  10225b:	85 c0                	test   eax,eax
  10225d:	74 28                	je     102287 <virtual_map.constprop.2+0x107>
        if (PHYSICAL_IS_USED(addr + i * PAGE_SIZE))
  10225f:	f6 05 00 a0 21 00 01 	test   BYTE PTR ds:0x21a000,0x1
  102266:	0f 85 84 00 00 00    	jne    1022f0 <virtual_map.constprop.2+0x170>
    log("P_S_USED ADDR=0x%x COUNT=%d", addr, count);
  10226c:	83 ec 04             	sub    esp,0x4
  10226f:	6a 01                	push   0x1
  102271:	6a 00                	push   0x0
  102273:	68 a7 20 11 00       	push   0x1120a7
  102278:	e8 23 f4 ff ff       	call   1016a0 <log>
        PHYSICAL_SET_USED(addr + (i * PAGE_SIZE));
  10227d:	80 0d 00 a0 21 00 01 	or     BYTE PTR ds:0x21a000,0x1
  102284:	83 c4 10             	add    esp,0x10
  102287:	31 ed                	xor    ebp,ebp
  102289:	31 db                	xor    ebx,ebx
    uint addr = physical_alloc(count);

    if (addr != 0)
        virtual_map(&kpdir, addr, addr, count, 0);

    atomic_end();
  10228b:	e8 d0 5b 00 00       	call   107e60 <atomic_end>
            log("New page table at %x", ptable);
  102290:	83 ec 08             	sub    esp,0x8
  102293:	53                   	push   ebx
  102294:	68 92 20 11 00       	push   0x112092
  102299:	e8 02 f4 ff ff       	call   1016a0 <log>
            pde->Present = 1;
  10229e:	8b 4c 24 2c          	mov    ecx,DWORD PTR [esp+0x2c]
            pde->User = user;
  1022a2:	0f b6 54 24 27       	movzx  edx,BYTE PTR [esp+0x27]
            pde->Present = 1;
  1022a7:	0f b6 04 8f          	movzx  eax,BYTE PTR [edi+ecx*4]
            pde->User = user;
  1022ab:	c1 e2 02             	shl    edx,0x2
            pde->Write = 1;
  1022ae:	83 c8 03             	or     eax,0x3
            pde->User = user;
  1022b1:	83 e0 fb             	and    eax,0xfffffffb
  1022b4:	09 d0                	or     eax,edx
            pde->PageFrameNumber = (u32)(ptable) >> 12;
  1022b6:	89 ea                	mov    edx,ebp
            pde->User = user;
  1022b8:	88 04 8f             	mov    BYTE PTR [edi+ecx*4],al
            pde->PageFrameNumber = (u32)(ptable) >> 12;
  1022bb:	0f b6 44 8f 01       	movzx  eax,BYTE PTR [edi+ecx*4+0x1]
  1022c0:	c1 e2 04             	shl    edx,0x4
  1022c3:	88 44 24 1c          	mov    BYTE PTR [esp+0x1c],al
  1022c7:	83 e0 0f             	and    eax,0xf
  1022ca:	83 c4 10             	add    esp,0x10
  1022cd:	09 d0                	or     eax,edx
  1022cf:	88 44 8f 01          	mov    BYTE PTR [edi+ecx*4+0x1],al
  1022d3:	89 e8                	mov    eax,ebp
  1022d5:	c1 ed 0c             	shr    ebp,0xc
  1022d8:	c1 e8 04             	shr    eax,0x4
  1022db:	88 44 8f 02          	mov    BYTE PTR [edi+ecx*4+0x2],al
  1022df:	89 e8                	mov    eax,ebp
  1022e1:	88 44 8f 03          	mov    BYTE PTR [edi+ecx*4+0x3],al
  1022e5:	e9 01 ff ff ff       	jmp    1021eb <virtual_map.constprop.2+0x6b>
  1022ea:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
  1022f0:	c1 e0 0c             	shl    eax,0xc
        if (PHYSICAL_IS_USED(addr + i * PAGE_SIZE))
  1022f3:	bb 00 10 00 00       	mov    ebx,0x1000
  1022f8:	89 44 24 0c          	mov    DWORD PTR [esp+0xc],eax
  1022fc:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
    for (uint i = 0; i < (TOTAL_MEMORY / PAGE_SIZE); i++)
  102300:	39 5c 24 0c          	cmp    DWORD PTR [esp+0xc],ebx
  102304:	74 81                	je     102287 <virtual_map.constprop.2+0x107>
  102306:	8d 83 00 10 00 00    	lea    eax,[ebx+0x1000]
        if (PHYSICAL_IS_USED(addr + i * PAGE_SIZE))
  10230c:	89 da                	mov    edx,ebx
  10230e:	89 dd                	mov    ebp,ebx
  102310:	c1 ea 0f             	shr    edx,0xf
  102313:	c1 ed 0c             	shr    ebp,0xc
  102316:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  10231a:	0f b6 82 00 a0 21 00 	movzx  eax,BYTE PTR [edx+0x21a000]
  102321:	89 e9                	mov    ecx,ebp
  102323:	83 e1 07             	and    ecx,0x7
  102326:	0f a3 c8             	bt     eax,ecx
  102329:	72 5d                	jb     102388 <virtual_map.constprop.2+0x208>
  10232b:	89 54 24 10          	mov    DWORD PTR [esp+0x10],edx
  10232f:	89 4c 24 0c          	mov    DWORD PTR [esp+0xc],ecx
    log("P_S_USED ADDR=0x%x COUNT=%d", addr, count);
  102333:	83 ec 04             	sub    esp,0x4
  102336:	6a 01                	push   0x1
  102338:	53                   	push   ebx
  102339:	81 e5 ff ff 0f 00    	and    ebp,0xfffff
  10233f:	68 a7 20 11 00       	push   0x1120a7
  102344:	e8 57 f3 ff ff       	call   1016a0 <log>
        PHYSICAL_SET_USED(addr + (i * PAGE_SIZE));
  102349:	8b 4c 24 1c          	mov    ecx,DWORD PTR [esp+0x1c]
  10234d:	8b 54 24 20          	mov    edx,DWORD PTR [esp+0x20]
  102351:	b8 01 00 00 00       	mov    eax,0x1
    if (addr != 0)
  102356:	83 c4 10             	add    esp,0x10
        PHYSICAL_SET_USED(addr + (i * PAGE_SIZE));
  102359:	d3 e0                	shl    eax,cl
  10235b:	08 82 00 a0 21 00    	or     BYTE PTR [edx+0x21a000],al
    if (addr != 0)
  102361:	85 db                	test   ebx,ebx
  102363:	0f 84 22 ff ff ff    	je     10228b <virtual_map.constprop.2+0x10b>
        virtual_map(&kpdir, addr, addr, count, 0);
  102369:	83 ec 0c             	sub    esp,0xc
  10236c:	89 d9                	mov    ecx,ebx
  10236e:	89 da                	mov    edx,ebx
  102370:	6a 00                	push   0x0
  102372:	b8 00 90 11 00       	mov    eax,0x119000
  102377:	e8 04 fe ff ff       	call   102180 <virtual_map.constprop.2>
  10237c:	83 c4 10             	add    esp,0x10
  10237f:	e9 07 ff ff ff       	jmp    10228b <virtual_map.constprop.2+0x10b>
  102384:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
        if (PHYSICAL_IS_USED(addr + i * PAGE_SIZE))
  102388:	8b 5c 24 10          	mov    ebx,DWORD PTR [esp+0x10]
  10238c:	e9 6f ff ff ff       	jmp    102300 <virtual_map.constprop.2+0x180>
  102391:	eb 0d                	jmp    1023a0 <physical_is_used>
  102393:	90                   	nop
  102394:	90                   	nop
  102395:	90                   	nop
  102396:	90                   	nop
  102397:	90                   	nop
  102398:	90                   	nop
  102399:	90                   	nop
  10239a:	90                   	nop
  10239b:	90                   	nop
  10239c:	90                   	nop
  10239d:	90                   	nop
  10239e:	90                   	nop
  10239f:	90                   	nop

001023a0 <physical_is_used>:
{
  1023a0:	57                   	push   edi
  1023a1:	56                   	push   esi
  1023a2:	53                   	push   ebx
  1023a3:	8b 7c 24 14          	mov    edi,DWORD PTR [esp+0x14]
  1023a7:	8b 5c 24 10          	mov    ebx,DWORD PTR [esp+0x10]
    for (uint i = 0; i < count; i++)
  1023ab:	85 ff                	test   edi,edi
  1023ad:	74 61                	je     102410 <physical_is_used+0x70>
        if (PHYSICAL_IS_USED(addr + i * PAGE_SIZE))
  1023af:	89 d8                	mov    eax,ebx
  1023b1:	89 d9                	mov    ecx,ebx
  1023b3:	c1 e8 0f             	shr    eax,0xf
  1023b6:	c1 e9 0c             	shr    ecx,0xc
  1023b9:	0f b6 80 00 a0 21 00 	movzx  eax,BYTE PTR [eax+0x21a000]
  1023c0:	83 e1 07             	and    ecx,0x7
  1023c3:	d3 f8                	sar    eax,cl
  1023c5:	83 e0 01             	and    eax,0x1
  1023c8:	75 36                	jne    102400 <physical_is_used+0x60>
  1023ca:	81 c3 00 10 00 00    	add    ebx,0x1000
    for (uint i = 0; i < count; i++)
  1023d0:	31 f6                	xor    esi,esi
  1023d2:	eb 25                	jmp    1023f9 <physical_is_used+0x59>
  1023d4:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
        if (PHYSICAL_IS_USED(addr + i * PAGE_SIZE))
  1023d8:	89 da                	mov    edx,ebx
  1023da:	89 d9                	mov    ecx,ebx
  1023dc:	81 c3 00 10 00 00    	add    ebx,0x1000
  1023e2:	c1 ea 0f             	shr    edx,0xf
  1023e5:	c1 e9 0c             	shr    ecx,0xc
  1023e8:	0f b6 92 00 a0 21 00 	movzx  edx,BYTE PTR [edx+0x21a000]
  1023ef:	83 e1 07             	and    ecx,0x7
  1023f2:	d3 fa                	sar    edx,cl
  1023f4:	83 e2 01             	and    edx,0x1
  1023f7:	75 0f                	jne    102408 <physical_is_used+0x68>
    for (uint i = 0; i < count; i++)
  1023f9:	83 c6 01             	add    esi,0x1
  1023fc:	39 f7                	cmp    edi,esi
  1023fe:	75 d8                	jne    1023d8 <physical_is_used+0x38>
}
  102400:	5b                   	pop    ebx
  102401:	5e                   	pop    esi
  102402:	5f                   	pop    edi
  102403:	c3                   	ret    
  102404:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
  102408:	5b                   	pop    ebx
            return 1;
  102409:	89 d0                	mov    eax,edx
}
  10240b:	5e                   	pop    esi
  10240c:	5f                   	pop    edi
  10240d:	c3                   	ret    
  10240e:	66 90                	xchg   ax,ax
    return 0;
  102410:	31 c0                	xor    eax,eax
  102412:	eb ec                	jmp    102400 <physical_is_used+0x60>
  102414:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
  10241a:	8d bf 00 00 00 00    	lea    edi,[edi+0x0]

00102420 <physical_set_used>:
{
  102420:	55                   	push   ebp
  102421:	57                   	push   edi
  102422:	56                   	push   esi
  102423:	53                   	push   ebx
  102424:	83 ec 10             	sub    esp,0x10
  102427:	8b 74 24 28          	mov    esi,DWORD PTR [esp+0x28]
  10242b:	8b 5c 24 24          	mov    ebx,DWORD PTR [esp+0x24]
    log("P_S_USED ADDR=0x%x COUNT=%d", addr, count);
  10242f:	56                   	push   esi
  102430:	53                   	push   ebx
  102431:	68 a7 20 11 00       	push   0x1120a7
  102436:	e8 65 f2 ff ff       	call   1016a0 <log>
    for (uint i = 0; i < count; i++)
  10243b:	83 c4 10             	add    esp,0x10
  10243e:	85 f6                	test   esi,esi
  102440:	74 34                	je     102476 <physical_set_used+0x56>
  102442:	31 c0                	xor    eax,eax
        PHYSICAL_SET_USED(addr + (i * PAGE_SIZE));
  102444:	bf 01 00 00 00       	mov    edi,0x1
  102449:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
  102450:	89 d9                	mov    ecx,ebx
  102452:	89 da                	mov    edx,ebx
  102454:	89 fd                	mov    ebp,edi
  102456:	c1 e9 0c             	shr    ecx,0xc
  102459:	c1 ea 0f             	shr    edx,0xf
    for (uint i = 0; i < count; i++)
  10245c:	83 c0 01             	add    eax,0x1
        PHYSICAL_SET_USED(addr + (i * PAGE_SIZE));
  10245f:	83 e1 07             	and    ecx,0x7
  102462:	81 c3 00 10 00 00    	add    ebx,0x1000
  102468:	d3 e5                	shl    ebp,cl
  10246a:	89 e9                	mov    ecx,ebp
  10246c:	08 8a 00 a0 21 00    	or     BYTE PTR [edx+0x21a000],cl
    for (uint i = 0; i < count; i++)
  102472:	39 c6                	cmp    esi,eax
  102474:	75 da                	jne    102450 <physical_set_used+0x30>
}
  102476:	83 c4 0c             	add    esp,0xc
  102479:	5b                   	pop    ebx
  10247a:	5e                   	pop    esi
  10247b:	5f                   	pop    edi
  10247c:	5d                   	pop    ebp
  10247d:	c3                   	ret    
  10247e:	66 90                	xchg   ax,ax

00102480 <physical_set_free>:
{
  102480:	55                   	push   ebp
  102481:	57                   	push   edi
  102482:	56                   	push   esi
  102483:	53                   	push   ebx
  102484:	83 ec 10             	sub    esp,0x10
  102487:	8b 7c 24 28          	mov    edi,DWORD PTR [esp+0x28]
  10248b:	8b 5c 24 24          	mov    ebx,DWORD PTR [esp+0x24]
    log("P_S_FREE ADDR=%d COUNT=%d", addr, count);
  10248f:	57                   	push   edi
  102490:	53                   	push   ebx
  102491:	68 c3 20 11 00       	push   0x1120c3
  102496:	e8 05 f2 ff ff       	call   1016a0 <log>
    for (uint i = 0; i < count; i++)
  10249b:	83 c4 10             	add    esp,0x10
  10249e:	85 ff                	test   edi,edi
  1024a0:	74 34                	je     1024d6 <physical_set_free+0x56>
  1024a2:	31 d2                	xor    edx,edx
        PHYSICAL_SET_FREE(addr + (i * PAGE_SIZE));
  1024a4:	bd 01 00 00 00       	mov    ebp,0x1
  1024a9:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
  1024b0:	89 d9                	mov    ecx,ebx
  1024b2:	89 de                	mov    esi,ebx
  1024b4:	89 e8                	mov    eax,ebp
  1024b6:	c1 e9 0c             	shr    ecx,0xc
  1024b9:	c1 ee 0f             	shr    esi,0xf
    for (uint i = 0; i < count; i++)
  1024bc:	83 c2 01             	add    edx,0x1
        PHYSICAL_SET_FREE(addr + (i * PAGE_SIZE));
  1024bf:	83 e1 07             	and    ecx,0x7
  1024c2:	81 c3 00 10 00 00    	add    ebx,0x1000
  1024c8:	d3 e0                	shl    eax,cl
  1024ca:	f7 d0                	not    eax
  1024cc:	20 86 00 a0 21 00    	and    BYTE PTR [esi+0x21a000],al
    for (uint i = 0; i < count; i++)
  1024d2:	39 d7                	cmp    edi,edx
  1024d4:	75 da                	jne    1024b0 <physical_set_free+0x30>
}
  1024d6:	83 c4 0c             	add    esp,0xc
  1024d9:	5b                   	pop    ebx
  1024da:	5e                   	pop    esi
  1024db:	5f                   	pop    edi
  1024dc:	5d                   	pop    ebp
  1024dd:	c3                   	ret    
  1024de:	66 90                	xchg   ax,ax

001024e0 <physical_alloc>:
{
  1024e0:	55                   	push   ebp
  1024e1:	57                   	push   edi
  1024e2:	56                   	push   esi
  1024e3:	53                   	push   ebx
  1024e4:	83 ec 24             	sub    esp,0x24
  1024e7:	8b 7c 24 38          	mov    edi,DWORD PTR [esp+0x38]
    log("PALLOC COUNT=%d", count);
  1024eb:	57                   	push   edi
  1024ec:	68 82 20 11 00       	push   0x112082
  1024f1:	e8 aa f1 ff ff       	call   1016a0 <log>
    for (uint i = 0; i < (TOTAL_MEMORY / PAGE_SIZE); i++)
  1024f6:	8b 2d 10 ae 23 00    	mov    ebp,DWORD PTR ds:0x23ae10
  1024fc:	83 c4 10             	add    esp,0x10
  1024ff:	c1 ed 0c             	shr    ebp,0xc
  102502:	85 ed                	test   ebp,ebp
  102504:	0f 84 b3 00 00 00    	je     1025bd <physical_alloc+0xdd>
  10250a:	c1 e5 0c             	shl    ebp,0xc
  10250d:	89 6c 24 0c          	mov    DWORD PTR [esp+0xc],ebp
  102511:	31 ed                	xor    ebp,ebp
  102513:	90                   	nop
  102514:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
    for (uint i = 0; i < count; i++)
  102518:	85 ff                	test   edi,edi
  10251a:	0f 84 d0 00 00 00    	je     1025f0 <physical_alloc+0x110>
        if (PHYSICAL_IS_USED(addr + i * PAGE_SIZE))
  102520:	89 e8                	mov    eax,ebp
  102522:	8d b5 00 10 00 00    	lea    esi,[ebp+0x1000]
  102528:	c1 e8 0f             	shr    eax,0xf
  10252b:	0f b6 90 00 a0 21 00 	movzx  edx,BYTE PTR [eax+0x21a000]
  102532:	89 e8                	mov    eax,ebp
  102534:	c1 e8 0c             	shr    eax,0xc
  102537:	83 e0 07             	and    eax,0x7
  10253a:	0f a3 c2             	bt     edx,eax
  10253d:	0f 82 8d 00 00 00    	jb     1025d0 <physical_alloc+0xf0>
  102543:	89 f0                	mov    eax,esi
    for (uint i = 0; i < count; i++)
  102545:	31 db                	xor    ebx,ebx
  102547:	eb 25                	jmp    10256e <physical_alloc+0x8e>
  102549:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
        if (PHYSICAL_IS_USED(addr + i * PAGE_SIZE))
  102550:	89 c2                	mov    edx,eax
  102552:	c1 ea 0f             	shr    edx,0xf
  102555:	0f b6 8a 00 a0 21 00 	movzx  ecx,BYTE PTR [edx+0x21a000]
  10255c:	89 c2                	mov    edx,eax
  10255e:	05 00 10 00 00       	add    eax,0x1000
  102563:	c1 ea 0c             	shr    edx,0xc
  102566:	83 e2 07             	and    edx,0x7
  102569:	0f a3 d1             	bt     ecx,edx
  10256c:	72 62                	jb     1025d0 <physical_alloc+0xf0>
    for (uint i = 0; i < count; i++)
  10256e:	83 c3 01             	add    ebx,0x1
  102571:	39 df                	cmp    edi,ebx
  102573:	75 db                	jne    102550 <physical_alloc+0x70>
    log("P_S_USED ADDR=0x%x COUNT=%d", addr, count);
  102575:	83 ec 04             	sub    esp,0x4
        PHYSICAL_SET_USED(addr + (i * PAGE_SIZE));
  102578:	bf 01 00 00 00       	mov    edi,0x1
    log("P_S_USED ADDR=0x%x COUNT=%d", addr, count);
  10257d:	53                   	push   ebx
  10257e:	55                   	push   ebp
  10257f:	68 a7 20 11 00       	push   0x1120a7
  102584:	e8 17 f1 ff ff       	call   1016a0 <log>
  102589:	83 c4 10             	add    esp,0x10
  10258c:	89 e8                	mov    eax,ebp
    for (uint i = 0; i < count; i++)
  10258e:	31 d2                	xor    edx,edx
  102590:	89 6c 24 0c          	mov    DWORD PTR [esp+0xc],ebp
        PHYSICAL_SET_USED(addr + (i * PAGE_SIZE));
  102594:	89 c1                	mov    ecx,eax
  102596:	89 c6                	mov    esi,eax
  102598:	89 fd                	mov    ebp,edi
  10259a:	c1 e9 0c             	shr    ecx,0xc
  10259d:	c1 ee 0f             	shr    esi,0xf
    for (uint i = 0; i < count; i++)
  1025a0:	83 c2 01             	add    edx,0x1
        PHYSICAL_SET_USED(addr + (i * PAGE_SIZE));
  1025a3:	83 e1 07             	and    ecx,0x7
  1025a6:	05 00 10 00 00       	add    eax,0x1000
  1025ab:	d3 e5                	shl    ebp,cl
  1025ad:	89 e9                	mov    ecx,ebp
  1025af:	08 8e 00 a0 21 00    	or     BYTE PTR [esi+0x21a000],cl
    for (uint i = 0; i < count; i++)
  1025b5:	39 d3                	cmp    ebx,edx
  1025b7:	75 db                	jne    102594 <physical_alloc+0xb4>
  1025b9:	8b 6c 24 0c          	mov    ebp,DWORD PTR [esp+0xc]
}
  1025bd:	83 c4 1c             	add    esp,0x1c
  1025c0:	89 e8                	mov    eax,ebp
  1025c2:	5b                   	pop    ebx
  1025c3:	5e                   	pop    esi
  1025c4:	5f                   	pop    edi
  1025c5:	5d                   	pop    ebp
  1025c6:	c3                   	ret    
  1025c7:	89 f6                	mov    esi,esi
  1025c9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
    for (uint i = 0; i < (TOTAL_MEMORY / PAGE_SIZE); i++)
  1025d0:	39 74 24 0c          	cmp    DWORD PTR [esp+0xc],esi
  1025d4:	89 f5                	mov    ebp,esi
  1025d6:	0f 85 3c ff ff ff    	jne    102518 <physical_alloc+0x38>
}
  1025dc:	83 c4 1c             	add    esp,0x1c
    return 0;
  1025df:	31 ed                	xor    ebp,ebp
}
  1025e1:	5b                   	pop    ebx
  1025e2:	89 e8                	mov    eax,ebp
  1025e4:	5e                   	pop    esi
  1025e5:	5f                   	pop    edi
  1025e6:	5d                   	pop    ebp
  1025e7:	c3                   	ret    
  1025e8:	90                   	nop
  1025e9:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
    log("P_S_USED ADDR=0x%x COUNT=%d", addr, count);
  1025f0:	83 ec 04             	sub    esp,0x4
  1025f3:	6a 00                	push   0x0
  1025f5:	55                   	push   ebp
  1025f6:	68 a7 20 11 00       	push   0x1120a7
  1025fb:	e8 a0 f0 ff ff       	call   1016a0 <log>
  102600:	83 c4 10             	add    esp,0x10
}
  102603:	89 e8                	mov    eax,ebp
  102605:	83 c4 1c             	add    esp,0x1c
  102608:	5b                   	pop    ebx
  102609:	5e                   	pop    esi
  10260a:	5f                   	pop    edi
  10260b:	5d                   	pop    ebp
  10260c:	c3                   	ret    
  10260d:	8d 76 00             	lea    esi,[esi+0x0]

00102610 <physical_free>:
{
  102610:	55                   	push   ebp
  102611:	57                   	push   edi
  102612:	56                   	push   esi
  102613:	53                   	push   ebx
  102614:	83 ec 10             	sub    esp,0x10
  102617:	8b 7c 24 28          	mov    edi,DWORD PTR [esp+0x28]
  10261b:	8b 5c 24 24          	mov    ebx,DWORD PTR [esp+0x24]
    log("P_S_FREE ADDR=%d COUNT=%d", addr, count);
  10261f:	57                   	push   edi
  102620:	53                   	push   ebx
  102621:	68 c3 20 11 00       	push   0x1120c3
  102626:	e8 75 f0 ff ff       	call   1016a0 <log>
    for (uint i = 0; i < count; i++)
  10262b:	83 c4 10             	add    esp,0x10
  10262e:	85 ff                	test   edi,edi
  102630:	74 34                	je     102666 <physical_free+0x56>
  102632:	31 d2                	xor    edx,edx
        PHYSICAL_SET_FREE(addr + (i * PAGE_SIZE));
  102634:	bd 01 00 00 00       	mov    ebp,0x1
  102639:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
  102640:	89 d9                	mov    ecx,ebx
  102642:	89 de                	mov    esi,ebx
  102644:	89 e8                	mov    eax,ebp
  102646:	c1 e9 0c             	shr    ecx,0xc
  102649:	c1 ee 0f             	shr    esi,0xf
    for (uint i = 0; i < count; i++)
  10264c:	83 c2 01             	add    edx,0x1
        PHYSICAL_SET_FREE(addr + (i * PAGE_SIZE));
  10264f:	83 e1 07             	and    ecx,0x7
  102652:	81 c3 00 10 00 00    	add    ebx,0x1000
  102658:	d3 e0                	shl    eax,cl
  10265a:	f7 d0                	not    eax
  10265c:	20 86 00 a0 21 00    	and    BYTE PTR [esi+0x21a000],al
    for (uint i = 0; i < count; i++)
  102662:	39 d7                	cmp    edi,edx
  102664:	75 da                	jne    102640 <physical_free+0x30>
}
  102666:	83 c4 0c             	add    esp,0xc
  102669:	5b                   	pop    ebx
  10266a:	5e                   	pop    esi
  10266b:	5f                   	pop    edi
  10266c:	5d                   	pop    ebp
  10266d:	c3                   	ret    
  10266e:	66 90                	xchg   ax,ax

00102670 <virtual_absent>:
{
  102670:	55                   	push   ebp
  102671:	57                   	push   edi
  102672:	56                   	push   esi
  102673:	53                   	push   ebx
  102674:	8b 7c 24 1c          	mov    edi,DWORD PTR [esp+0x1c]
  102678:	8b 74 24 14          	mov    esi,DWORD PTR [esp+0x14]
  10267c:	8b 54 24 18          	mov    edx,DWORD PTR [esp+0x18]
    for (uint i = 0; i < count; i++)
  102680:	85 ff                	test   edi,edi
  102682:	74 6c                	je     1026f0 <virtual_absent+0x80>
  102684:	31 c9                	xor    ecx,ecx
    int absent = 1;
  102686:	b8 01 00 00 00       	mov    eax,0x1
  10268b:	90                   	nop
  10268c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
        uint pdi = PD_INDEX(vaddr + offset);
  102690:	89 d3                	mov    ebx,edx
  102692:	c1 eb 16             	shr    ebx,0x16
        if (pde->Present)
  102695:	f6 04 9e 01          	test   BYTE PTR [esi+ebx*4],0x1
  102699:	74 3b                	je     1026d6 <virtual_absent+0x66>
            absent = absent && !p->Present;
  10269b:	85 c0                	test   eax,eax
  10269d:	74 37                	je     1026d6 <virtual_absent+0x66>
            page_table_t *ptable = (page_table_t *)(pde->PageFrameNumber * PAGE_SIZE);
  10269f:	0f b6 44 9e 01       	movzx  eax,BYTE PTR [esi+ebx*4+0x1]
  1026a4:	c0 e8 04             	shr    al,0x4
  1026a7:	0f b6 e8             	movzx  ebp,al
  1026aa:	0f b6 44 9e 02       	movzx  eax,BYTE PTR [esi+ebx*4+0x2]
  1026af:	c1 e0 04             	shl    eax,0x4
  1026b2:	09 c5                	or     ebp,eax
  1026b4:	0f b6 44 9e 03       	movzx  eax,BYTE PTR [esi+ebx*4+0x3]
        uint pti = PT_INDEX(vaddr + offset);
  1026b9:	89 d3                	mov    ebx,edx
  1026bb:	c1 eb 0c             	shr    ebx,0xc
  1026be:	81 e3 ff 03 00 00    	and    ebx,0x3ff
            page_table_t *ptable = (page_table_t *)(pde->PageFrameNumber * PAGE_SIZE);
  1026c4:	c1 e0 0c             	shl    eax,0xc
  1026c7:	09 e8                	or     eax,ebp
            absent = absent && !p->Present;
  1026c9:	c1 e0 0c             	shl    eax,0xc
  1026cc:	0f b6 04 98          	movzx  eax,BYTE PTR [eax+ebx*4]
  1026d0:	83 f0 01             	xor    eax,0x1
  1026d3:	83 e0 01             	and    eax,0x1
    for (uint i = 0; i < count; i++)
  1026d6:	83 c1 01             	add    ecx,0x1
  1026d9:	81 c2 00 10 00 00    	add    edx,0x1000
  1026df:	39 cf                	cmp    edi,ecx
  1026e1:	75 ad                	jne    102690 <virtual_absent+0x20>
}
  1026e3:	5b                   	pop    ebx
  1026e4:	5e                   	pop    esi
  1026e5:	5f                   	pop    edi
  1026e6:	5d                   	pop    ebp
  1026e7:	c3                   	ret    
  1026e8:	90                   	nop
  1026e9:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
  1026f0:	5b                   	pop    ebx
    int absent = 1;
  1026f1:	b8 01 00 00 00       	mov    eax,0x1
}
  1026f6:	5e                   	pop    esi
  1026f7:	5f                   	pop    edi
  1026f8:	5d                   	pop    ebp
  1026f9:	c3                   	ret    
  1026fa:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]

00102700 <virtual_present>:
{
  102700:	55                   	push   ebp
  102701:	57                   	push   edi
  102702:	56                   	push   esi
  102703:	53                   	push   ebx
  102704:	83 ec 0c             	sub    esp,0xc
    for (uint i = 0; i < count; i++)
  102707:	8b 44 24 28          	mov    eax,DWORD PTR [esp+0x28]
{
  10270b:	8b 5c 24 20          	mov    ebx,DWORD PTR [esp+0x20]
    for (uint i = 0; i < count; i++)
  10270f:	85 c0                	test   eax,eax
  102711:	0f 84 ae 00 00 00    	je     1027c5 <virtual_present+0xc5>
        uint pdi = PD_INDEX(vaddr + offset);
  102717:	8b 7c 24 24          	mov    edi,DWORD PTR [esp+0x24]
        uint pti = PT_INDEX(vaddr + offset);
  10271b:	8b 54 24 24          	mov    edx,DWORD PTR [esp+0x24]
        uint pdi = PD_INDEX(vaddr + offset);
  10271f:	c1 ef 16             	shr    edi,0x16
        uint pti = PT_INDEX(vaddr + offset);
  102722:	c1 ea 0c             	shr    edx,0xc
  102725:	81 e2 ff 03 00 00    	and    edx,0x3ff
        if (!pde->Present)
  10272b:	f6 04 bb 01          	test   BYTE PTR [ebx+edi*4],0x1
  10272f:	0f 84 bb 00 00 00    	je     1027f0 <virtual_present+0xf0>
        page_table_t *ptable = (page_table_t *)(pde->PageFrameNumber * PAGE_SIZE);
  102735:	0f b6 4c bb 01       	movzx  ecx,BYTE PTR [ebx+edi*4+0x1]
  10273a:	0f b6 44 bb 02       	movzx  eax,BYTE PTR [ebx+edi*4+0x2]
  10273f:	c0 e9 04             	shr    cl,0x4
  102742:	c1 e0 04             	shl    eax,0x4
  102745:	0f b6 f1             	movzx  esi,cl
  102748:	09 f0                	or     eax,esi
  10274a:	89 c1                	mov    ecx,eax
  10274c:	0f b6 44 bb 03       	movzx  eax,BYTE PTR [ebx+edi*4+0x3]
  102751:	c1 e0 0c             	shl    eax,0xc
  102754:	09 c8                	or     eax,ecx
        if (!p->Present)
  102756:	c1 e0 0c             	shl    eax,0xc
  102759:	f6 04 90 01          	test   BYTE PTR [eax+edx*4],0x1
  10275d:	0f 84 8d 00 00 00    	je     1027f0 <virtual_present+0xf0>
  102763:	8b 44 24 24          	mov    eax,DWORD PTR [esp+0x24]
    for (uint i = 0; i < count; i++)
  102767:	31 ed                	xor    ebp,ebp
  102769:	8d b0 00 10 00 00    	lea    esi,[eax+0x1000]
  10276f:	eb 4b                	jmp    1027bc <virtual_present+0xbc>
  102771:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
        uint pdi = PD_INDEX(vaddr + offset);
  102778:	89 f1                	mov    ecx,esi
        uint pti = PT_INDEX(vaddr + offset);
  10277a:	89 f7                	mov    edi,esi
  10277c:	c1 ef 0c             	shr    edi,0xc
        uint pdi = PD_INDEX(vaddr + offset);
  10277f:	c1 e9 16             	shr    ecx,0x16
        uint pti = PT_INDEX(vaddr + offset);
  102782:	81 e7 ff 03 00 00    	and    edi,0x3ff
        if (!pde->Present)
  102788:	f6 04 8b 01          	test   BYTE PTR [ebx+ecx*4],0x1
  10278c:	74 62                	je     1027f0 <virtual_present+0xf0>
        page_table_t *ptable = (page_table_t *)(pde->PageFrameNumber * PAGE_SIZE);
  10278e:	0f b6 54 8b 01       	movzx  edx,BYTE PTR [ebx+ecx*4+0x1]
  102793:	0f b6 44 8b 02       	movzx  eax,BYTE PTR [ebx+ecx*4+0x2]
  102798:	81 c6 00 10 00 00    	add    esi,0x1000
  10279e:	c0 ea 04             	shr    dl,0x4
  1027a1:	c1 e0 04             	shl    eax,0x4
  1027a4:	0f b6 d2             	movzx  edx,dl
  1027a7:	09 c2                	or     edx,eax
  1027a9:	0f b6 44 8b 03       	movzx  eax,BYTE PTR [ebx+ecx*4+0x3]
  1027ae:	c1 e0 0c             	shl    eax,0xc
  1027b1:	09 d0                	or     eax,edx
        if (!p->Present)
  1027b3:	c1 e0 0c             	shl    eax,0xc
  1027b6:	f6 04 b8 01          	test   BYTE PTR [eax+edi*4],0x1
  1027ba:	74 34                	je     1027f0 <virtual_present+0xf0>
    for (uint i = 0; i < count; i++)
  1027bc:	83 c5 01             	add    ebp,0x1
  1027bf:	39 6c 24 28          	cmp    DWORD PTR [esp+0x28],ebp
  1027c3:	75 b3                	jne    102778 <virtual_present+0x78>
    log("0x%x present!", vaddr);
  1027c5:	83 ec 08             	sub    esp,0x8
  1027c8:	ff 74 24 2c          	push   DWORD PTR [esp+0x2c]
  1027cc:	68 ea 20 11 00       	push   0x1120ea
  1027d1:	e8 ca ee ff ff       	call   1016a0 <log>
    return 1;
  1027d6:	83 c4 10             	add    esp,0x10
  1027d9:	b8 01 00 00 00       	mov    eax,0x1
}
  1027de:	83 c4 0c             	add    esp,0xc
  1027e1:	5b                   	pop    ebx
  1027e2:	5e                   	pop    esi
  1027e3:	5f                   	pop    edi
  1027e4:	5d                   	pop    ebp
  1027e5:	c3                   	ret    
  1027e6:	8d 76 00             	lea    esi,[esi+0x0]
  1027e9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
            log("0x%x absent!", vaddr);
  1027f0:	83 ec 08             	sub    esp,0x8
  1027f3:	ff 74 24 2c          	push   DWORD PTR [esp+0x2c]
  1027f7:	68 dd 20 11 00       	push   0x1120dd
  1027fc:	e8 9f ee ff ff       	call   1016a0 <log>
            return 0;
  102801:	83 c4 10             	add    esp,0x10
  102804:	31 c0                	xor    eax,eax
}
  102806:	83 c4 0c             	add    esp,0xc
  102809:	5b                   	pop    ebx
  10280a:	5e                   	pop    esi
  10280b:	5f                   	pop    edi
  10280c:	5d                   	pop    ebp
  10280d:	c3                   	ret    
  10280e:	66 90                	xchg   ax,ax

00102810 <virtual2physical>:
{
  102810:	56                   	push   esi
  102811:	53                   	push   ebx
  102812:	8b 4c 24 10          	mov    ecx,DWORD PTR [esp+0x10]
  102816:	8b 5c 24 0c          	mov    ebx,DWORD PTR [esp+0xc]
    uint pdi = PD_INDEX(vaddr);
  10281a:	89 ce                	mov    esi,ecx
  10281c:	c1 ee 16             	shr    esi,0x16
    page_table_t *ptable = (page_table_t *)(pde->PageFrameNumber * PAGE_SIZE);
  10281f:	0f b6 44 b3 01       	movzx  eax,BYTE PTR [ebx+esi*4+0x1]
  102824:	0f b6 54 b3 02       	movzx  edx,BYTE PTR [ebx+esi*4+0x2]
  102829:	c0 e8 04             	shr    al,0x4
  10282c:	c1 e2 04             	shl    edx,0x4
  10282f:	0f b6 c0             	movzx  eax,al
  102832:	09 d0                	or     eax,edx
  102834:	0f b6 54 b3 03       	movzx  edx,BYTE PTR [ebx+esi*4+0x3]
}
  102839:	5b                   	pop    ebx
  10283a:	5e                   	pop    esi
    page_table_t *ptable = (page_table_t *)(pde->PageFrameNumber * PAGE_SIZE);
  10283b:	c1 e2 0c             	shl    edx,0xc
  10283e:	09 c2                	or     edx,eax
    uint pti = PT_INDEX(vaddr);
  102840:	89 c8                	mov    eax,ecx
    return ((p->PageFrameNumber & ~0xfff) + (vaddr & 0xfff));
  102842:	81 e1 ff 0f 00 00    	and    ecx,0xfff
    uint pti = PT_INDEX(vaddr);
  102848:	c1 e8 0c             	shr    eax,0xc
    return ((p->PageFrameNumber & ~0xfff) + (vaddr & 0xfff));
  10284b:	c1 e2 0c             	shl    edx,0xc
    uint pti = PT_INDEX(vaddr);
  10284e:	25 ff 03 00 00       	and    eax,0x3ff
    return ((p->PageFrameNumber & ~0xfff) + (vaddr & 0xfff));
  102853:	8b 04 82             	mov    eax,DWORD PTR [edx+eax*4]
  102856:	c1 e8 0c             	shr    eax,0xc
  102859:	25 00 f0 ff ff       	and    eax,0xfffff000
  10285e:	09 c8                	or     eax,ecx
}
  102860:	c3                   	ret    
  102861:	eb 0d                	jmp    102870 <virtual_map>
  102863:	90                   	nop
  102864:	90                   	nop
  102865:	90                   	nop
  102866:	90                   	nop
  102867:	90                   	nop
  102868:	90                   	nop
  102869:	90                   	nop
  10286a:	90                   	nop
  10286b:	90                   	nop
  10286c:	90                   	nop
  10286d:	90                   	nop
  10286e:	90                   	nop
  10286f:	90                   	nop

00102870 <virtual_map>:
{
  102870:	55                   	push   ebp
  102871:	57                   	push   edi
  102872:	56                   	push   esi
  102873:	53                   	push   ebx
  102874:	83 ec 2c             	sub    esp,0x2c
  102877:	8b 5c 24 50          	mov    ebx,DWORD PTR [esp+0x50]
    log("VMAP VADDR=0x%x PADDR=0x%x COUNT=%d", vaddr, paddr, count);
  10287b:	ff 74 24 4c          	push   DWORD PTR [esp+0x4c]
  10287f:	ff 74 24 4c          	push   DWORD PTR [esp+0x4c]
  102883:	ff 74 24 4c          	push   DWORD PTR [esp+0x4c]
  102887:	68 b4 65 11 00       	push   0x1165b4
  10288c:	e8 0f ee ff ff       	call   1016a0 <log>
    for (uint i = 0; i < count; i++)
  102891:	83 c4 10             	add    esp,0x10
  102894:	8b 4c 24 4c          	mov    ecx,DWORD PTR [esp+0x4c]
  102898:	85 c9                	test   ecx,ecx
  10289a:	0f 84 78 01 00 00    	je     102a18 <virtual_map+0x1a8>
  1028a0:	8b 44 24 44          	mov    eax,DWORD PTR [esp+0x44]
  1028a4:	83 e3 01             	and    ebx,0x1
  1028a7:	c7 44 24 0c 00 00 00 	mov    DWORD PTR [esp+0xc],0x0
  1028ae:	00 
  1028af:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
            pde->User = user;
  1028b3:	8d 04 9d 00 00 00 00 	lea    eax,[ebx*4+0x0]
  1028ba:	88 44 24 1f          	mov    BYTE PTR [esp+0x1f],al
  1028be:	eb 7b                	jmp    10293b <virtual_map+0xcb>
  1028c0:	89 c7                	mov    edi,eax
        page_table_t *ptable = (page_table_t *)(pde->PageFrameNumber * PAGE_SIZE);
  1028c2:	0f b6 44 b0 01       	movzx  eax,BYTE PTR [eax+esi*4+0x1]
  1028c7:	88 44 24 14          	mov    BYTE PTR [esp+0x14],al
  1028cb:	0f b6 5c b7 02       	movzx  ebx,BYTE PTR [edi+esi*4+0x2]
  1028d0:	c0 e8 04             	shr    al,0x4
  1028d3:	0f b6 c0             	movzx  eax,al
  1028d6:	c1 e3 04             	shl    ebx,0x4
  1028d9:	09 d8                	or     eax,ebx
  1028db:	0f b6 5c b7 03       	movzx  ebx,BYTE PTR [edi+esi*4+0x3]
  1028e0:	c1 e3 0c             	shl    ebx,0xc
  1028e3:	09 c3                	or     ebx,eax
  1028e5:	c1 e3 0c             	shl    ebx,0xc
        p->Present = 1;
  1028e8:	0f b6 04 ab          	movzx  eax,BYTE PTR [ebx+ebp*4]
        p->PageFrameNumber = (paddr + offset) >> 12;
  1028ec:	8b 74 24 10          	mov    esi,DWORD PTR [esp+0x10]
        p->Present = 1;
  1028f0:	83 c8 01             	or     eax,0x1
        p->User = user;
  1028f3:	83 e0 fb             	and    eax,0xfffffffb
  1028f6:	0a 44 24 1f          	or     al,BYTE PTR [esp+0x1f]
        p->Write = 1;
  1028fa:	83 c8 02             	or     eax,0x2
  1028fd:	88 04 ab             	mov    BYTE PTR [ebx+ebp*4],al
        p->PageFrameNumber = (paddr + offset) >> 12;
  102900:	8b 44 24 48          	mov    eax,DWORD PTR [esp+0x48]
  102904:	2b 44 24 44          	sub    eax,DWORD PTR [esp+0x44]
  102908:	01 f0                	add    eax,esi
  10290a:	81 c6 00 10 00 00    	add    esi,0x1000
  102910:	25 00 f0 ff ff       	and    eax,0xfffff000
  102915:	89 c2                	mov    edx,eax
  102917:	8b 04 ab             	mov    eax,DWORD PTR [ebx+ebp*4]
  10291a:	25 ff 0f 00 00       	and    eax,0xfff
  10291f:	09 d0                	or     eax,edx
  102921:	89 04 ab             	mov    DWORD PTR [ebx+ebp*4],eax
    for (uint i = 0; i < count; i++)
  102924:	83 44 24 0c 01       	add    DWORD PTR [esp+0xc],0x1
  102929:	8b 44 24 0c          	mov    eax,DWORD PTR [esp+0xc]
  10292d:	39 44 24 4c          	cmp    DWORD PTR [esp+0x4c],eax
  102931:	89 74 24 10          	mov    DWORD PTR [esp+0x10],esi
  102935:	0f 84 dd 00 00 00    	je     102a18 <virtual_map+0x1a8>
        uint pdi = PD_INDEX(vaddr + offset);
  10293b:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  10293f:	89 c6                	mov    esi,eax
        uint pti = PT_INDEX(vaddr + offset);
  102941:	c1 e8 0c             	shr    eax,0xc
  102944:	25 ff 03 00 00       	and    eax,0x3ff
        uint pdi = PD_INDEX(vaddr + offset);
  102949:	c1 ee 16             	shr    esi,0x16
        uint pti = PT_INDEX(vaddr + offset);
  10294c:	89 c5                	mov    ebp,eax
        if (!pde->Present)
  10294e:	8b 44 24 40          	mov    eax,DWORD PTR [esp+0x40]
  102952:	f6 04 b0 01          	test   BYTE PTR [eax+esi*4],0x1
  102956:	0f 85 64 ff ff ff    	jne    1028c0 <virtual_map+0x50>
            log("Missing page table! Allocating a new one");
  10295c:	83 ec 0c             	sub    esp,0xc
  10295f:	68 d8 65 11 00       	push   0x1165d8
  102964:	e8 37 ed ff ff       	call   1016a0 <log>
    atomic_begin();
  102969:	e8 d2 54 00 00       	call   107e40 <atomic_begin>
    log("PALLOC COUNT=%d", count);
  10296e:	58                   	pop    eax
  10296f:	5a                   	pop    edx
  102970:	6a 01                	push   0x1
  102972:	68 82 20 11 00       	push   0x112082
  102977:	e8 24 ed ff ff       	call   1016a0 <log>
    for (uint i = 0; i < (TOTAL_MEMORY / PAGE_SIZE); i++)
  10297c:	8b 15 10 ae 23 00    	mov    edx,DWORD PTR ds:0x23ae10
  102982:	83 c4 10             	add    esp,0x10
  102985:	c1 ea 0c             	shr    edx,0xc
  102988:	85 d2                	test   edx,edx
  10298a:	74 28                	je     1029b4 <virtual_map+0x144>
        if (PHYSICAL_IS_USED(addr + i * PAGE_SIZE))
  10298c:	f6 05 00 a0 21 00 01 	test   BYTE PTR ds:0x21a000,0x1
  102993:	0f 85 97 00 00 00    	jne    102a30 <virtual_map+0x1c0>
    log("P_S_USED ADDR=0x%x COUNT=%d", addr, count);
  102999:	83 ec 04             	sub    esp,0x4
  10299c:	6a 01                	push   0x1
  10299e:	6a 00                	push   0x0
  1029a0:	68 a7 20 11 00       	push   0x1120a7
  1029a5:	e8 f6 ec ff ff       	call   1016a0 <log>
        PHYSICAL_SET_USED(addr + (i * PAGE_SIZE));
  1029aa:	80 0d 00 a0 21 00 01 	or     BYTE PTR ds:0x21a000,0x1
  1029b1:	83 c4 10             	add    esp,0x10
  1029b4:	31 db                	xor    ebx,ebx
  1029b6:	31 ff                	xor    edi,edi
    atomic_end();
  1029b8:	e8 a3 54 00 00       	call   107e60 <atomic_end>
            log("New page table at %x", ptable);
  1029bd:	83 ec 08             	sub    esp,0x8
  1029c0:	53                   	push   ebx
  1029c1:	68 92 20 11 00       	push   0x112092
  1029c6:	e8 d5 ec ff ff       	call   1016a0 <log>
            pde->Present = 1;
  1029cb:	8b 44 24 50          	mov    eax,DWORD PTR [esp+0x50]
            pde->User = user;
  1029cf:	8b 4c 24 50          	mov    ecx,DWORD PTR [esp+0x50]
            pde->PageFrameNumber = (u32)(ptable) >> 12;
  1029d3:	89 fa                	mov    edx,edi
  1029d5:	c1 e2 04             	shl    edx,0x4
            pde->Present = 1;
  1029d8:	0f b6 04 b0          	movzx  eax,BYTE PTR [eax+esi*4]
            pde->Write = 1;
  1029dc:	83 c8 03             	or     eax,0x3
            pde->User = user;
  1029df:	83 e0 fb             	and    eax,0xfffffffb
  1029e2:	0a 44 24 2f          	or     al,BYTE PTR [esp+0x2f]
  1029e6:	88 04 b1             	mov    BYTE PTR [ecx+esi*4],al
            pde->PageFrameNumber = (u32)(ptable) >> 12;
  1029e9:	0f b6 44 b1 01       	movzx  eax,BYTE PTR [ecx+esi*4+0x1]
  1029ee:	88 44 24 24          	mov    BYTE PTR [esp+0x24],al
  1029f2:	83 e0 0f             	and    eax,0xf
  1029f5:	83 c4 10             	add    esp,0x10
  1029f8:	09 d0                	or     eax,edx
  1029fa:	88 44 b1 01          	mov    BYTE PTR [ecx+esi*4+0x1],al
  1029fe:	89 f8                	mov    eax,edi
  102a00:	c1 ef 0c             	shr    edi,0xc
  102a03:	c1 e8 04             	shr    eax,0x4
  102a06:	88 44 b1 02          	mov    BYTE PTR [ecx+esi*4+0x2],al
  102a0a:	89 f8                	mov    eax,edi
  102a0c:	88 44 b1 03          	mov    BYTE PTR [ecx+esi*4+0x3],al
  102a10:	e9 d3 fe ff ff       	jmp    1028e8 <virtual_map+0x78>
  102a15:	8d 76 00             	lea    esi,[esi+0x0]
    paging_invalidate_tlb();
  102a18:	e8 07 2a 00 00       	call   105424 <paging_invalidate_tlb>
}
  102a1d:	83 c4 2c             	add    esp,0x2c
  102a20:	31 c0                	xor    eax,eax
  102a22:	5b                   	pop    ebx
  102a23:	5e                   	pop    esi
  102a24:	5f                   	pop    edi
  102a25:	5d                   	pop    ebp
  102a26:	c3                   	ret    
  102a27:	89 f6                	mov    esi,esi
  102a29:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
  102a30:	c1 e2 0c             	shl    edx,0xc
        if (PHYSICAL_IS_USED(addr + i * PAGE_SIZE))
  102a33:	bb 00 10 00 00       	mov    ebx,0x1000
  102a38:	89 54 24 14          	mov    DWORD PTR [esp+0x14],edx
  102a3c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
    for (uint i = 0; i < (TOTAL_MEMORY / PAGE_SIZE); i++)
  102a40:	39 5c 24 14          	cmp    DWORD PTR [esp+0x14],ebx
  102a44:	0f 84 6a ff ff ff    	je     1029b4 <virtual_map+0x144>
  102a4a:	8d 83 00 10 00 00    	lea    eax,[ebx+0x1000]
        if (PHYSICAL_IS_USED(addr + i * PAGE_SIZE))
  102a50:	89 da                	mov    edx,ebx
  102a52:	89 df                	mov    edi,ebx
  102a54:	c1 ea 0f             	shr    edx,0xf
  102a57:	c1 ef 0c             	shr    edi,0xc
  102a5a:	89 44 24 18          	mov    DWORD PTR [esp+0x18],eax
  102a5e:	0f b6 82 00 a0 21 00 	movzx  eax,BYTE PTR [edx+0x21a000]
  102a65:	89 f9                	mov    ecx,edi
  102a67:	83 e1 07             	and    ecx,0x7
  102a6a:	0f a3 c8             	bt     eax,ecx
  102a6d:	72 61                	jb     102ad0 <virtual_map+0x260>
  102a6f:	89 54 24 18          	mov    DWORD PTR [esp+0x18],edx
  102a73:	89 4c 24 14          	mov    DWORD PTR [esp+0x14],ecx
    log("P_S_USED ADDR=0x%x COUNT=%d", addr, count);
  102a77:	83 ec 04             	sub    esp,0x4
  102a7a:	6a 01                	push   0x1
  102a7c:	53                   	push   ebx
  102a7d:	81 e7 ff ff 0f 00    	and    edi,0xfffff
  102a83:	68 a7 20 11 00       	push   0x1120a7
  102a88:	e8 13 ec ff ff       	call   1016a0 <log>
        PHYSICAL_SET_USED(addr + (i * PAGE_SIZE));
  102a8d:	8b 4c 24 24          	mov    ecx,DWORD PTR [esp+0x24]
  102a91:	8b 54 24 28          	mov    edx,DWORD PTR [esp+0x28]
  102a95:	b8 01 00 00 00       	mov    eax,0x1
    if (addr != 0)
  102a9a:	83 c4 10             	add    esp,0x10
        PHYSICAL_SET_USED(addr + (i * PAGE_SIZE));
  102a9d:	d3 e0                	shl    eax,cl
  102a9f:	08 82 00 a0 21 00    	or     BYTE PTR [edx+0x21a000],al
    if (addr != 0)
  102aa5:	85 db                	test   ebx,ebx
  102aa7:	0f 84 0b ff ff ff    	je     1029b8 <virtual_map+0x148>
        virtual_map(&kpdir, addr, addr, count, 0);
  102aad:	83 ec 0c             	sub    esp,0xc
  102ab0:	89 d9                	mov    ecx,ebx
  102ab2:	89 da                	mov    edx,ebx
  102ab4:	6a 00                	push   0x0
  102ab6:	b8 00 90 11 00       	mov    eax,0x119000
  102abb:	e8 c0 f6 ff ff       	call   102180 <virtual_map.constprop.2>
  102ac0:	83 c4 10             	add    esp,0x10
  102ac3:	e9 f0 fe ff ff       	jmp    1029b8 <virtual_map+0x148>
  102ac8:	90                   	nop
  102ac9:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
        if (PHYSICAL_IS_USED(addr + i * PAGE_SIZE))
  102ad0:	8b 5c 24 18          	mov    ebx,DWORD PTR [esp+0x18]
  102ad4:	e9 67 ff ff ff       	jmp    102a40 <virtual_map+0x1d0>
  102ad9:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]

00102ae0 <virtual_unmap>:
{
  102ae0:	55                   	push   ebp
  102ae1:	57                   	push   edi
  102ae2:	56                   	push   esi
  102ae3:	53                   	push   ebx
  102ae4:	8b 74 24 1c          	mov    esi,DWORD PTR [esp+0x1c]
  102ae8:	8b 5c 24 14          	mov    ebx,DWORD PTR [esp+0x14]
  102aec:	8b 54 24 18          	mov    edx,DWORD PTR [esp+0x18]
    for (uint i = 0; i < count; i++)
  102af0:	85 f6                	test   esi,esi
  102af2:	74 58                	je     102b4c <virtual_unmap+0x6c>
  102af4:	31 c9                	xor    ecx,ecx
  102af6:	8d 76 00             	lea    esi,[esi+0x0]
  102af9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
        uint pdi = PD_INDEX(vaddr + offset);
  102b00:	89 d5                	mov    ebp,edx
  102b02:	c1 ed 16             	shr    ebp,0x16
        if (pde->Present)
  102b05:	f6 04 ab 01          	test   BYTE PTR [ebx+ebp*4],0x1
  102b09:	74 34                	je     102b3f <virtual_unmap+0x5f>
        page_table_t *ptable = (page_table_t *)(pde->PageFrameNumber * PAGE_SIZE);
  102b0b:	0f b6 44 ab 01       	movzx  eax,BYTE PTR [ebx+ebp*4+0x1]
  102b10:	c0 e8 04             	shr    al,0x4
  102b13:	0f b6 f8             	movzx  edi,al
  102b16:	0f b6 44 ab 02       	movzx  eax,BYTE PTR [ebx+ebp*4+0x2]
  102b1b:	c1 e0 04             	shl    eax,0x4
  102b1e:	09 c7                	or     edi,eax
  102b20:	0f b6 44 ab 03       	movzx  eax,BYTE PTR [ebx+ebp*4+0x3]
  102b25:	c1 e0 0c             	shl    eax,0xc
  102b28:	09 f8                	or     eax,edi
        uint pti = PT_INDEX(vaddr + offset);
  102b2a:	89 d7                	mov    edi,edx
  102b2c:	c1 ef 0c             	shr    edi,0xc
            p->as_uint = 0;
  102b2f:	c1 e0 0c             	shl    eax,0xc
        uint pti = PT_INDEX(vaddr + offset);
  102b32:	81 e7 ff 03 00 00    	and    edi,0x3ff
            p->as_uint = 0;
  102b38:	c7 04 b8 00 00 00 00 	mov    DWORD PTR [eax+edi*4],0x0
    for (uint i = 0; i < count; i++)
  102b3f:	83 c1 01             	add    ecx,0x1
  102b42:	81 c2 00 10 00 00    	add    edx,0x1000
  102b48:	39 ce                	cmp    esi,ecx
  102b4a:	75 b4                	jne    102b00 <virtual_unmap+0x20>
}
  102b4c:	5b                   	pop    ebx
  102b4d:	5e                   	pop    esi
  102b4e:	5f                   	pop    edi
  102b4f:	5d                   	pop    ebp
    paging_invalidate_tlb();
  102b50:	e9 cf 28 00 00       	jmp    105424 <paging_invalidate_tlb>
  102b55:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
  102b59:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00102b60 <memory_setup>:
{
  102b60:	57                   	push   edi
  102b61:	56                   	push   esi
  102b62:	53                   	push   ebx
  102b63:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  102b67:	8b 5c 24 10          	mov    ebx,DWORD PTR [esp+0x10]
    log("Memory: USED=%dko TOTAL=%dko", used / 1024, total / 1024);
  102b6b:	83 ec 04             	sub    esp,0x4
    TOTAL_MEMORY = total;
  102b6e:	a3 10 ae 23 00       	mov    ds:0x23ae10,eax
    log("Memory: USED=%dko TOTAL=%dko", used / 1024, total / 1024);
  102b73:	c1 e8 0a             	shr    eax,0xa
  102b76:	50                   	push   eax
  102b77:	89 d8                	mov    eax,ebx
  102b79:	c1 e8 0a             	shr    eax,0xa
  102b7c:	50                   	push   eax
  102b7d:	68 f8 20 11 00       	push   0x1120f8
  102b82:	e8 19 eb ff ff       	call   1016a0 <log>
  102b87:	b9 00 a0 11 00       	mov    ecx,0x11a000
  102b8c:	83 c4 10             	add    esp,0x10
    for (uint i = 0; i < 256; i++)
  102b8f:	31 c0                	xor    eax,eax
  102b91:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
        e->User = 0;
  102b98:	0f b6 14 85 00 90 11 	movzx  edx,BYTE PTR [eax*4+0x119000]
  102b9f:	00 
        e->PageFrameNumber = (uint)&kptable[i] / PAGE_SIZE;
  102ba0:	89 ce                	mov    esi,ecx
  102ba2:	81 c1 00 10 00 00    	add    ecx,0x1000
  102ba8:	81 e6 00 f0 ff ff    	and    esi,0xfffff000
        e->User = 0;
  102bae:	83 e2 fb             	and    edx,0xfffffffb
        e->Present = 1;
  102bb1:	83 ca 03             	or     edx,0x3
  102bb4:	88 14 85 00 90 11 00 	mov    BYTE PTR [eax*4+0x119000],dl
        e->PageFrameNumber = (uint)&kptable[i] / PAGE_SIZE;
  102bbb:	8b 14 85 00 90 11 00 	mov    edx,DWORD PTR [eax*4+0x119000]
  102bc2:	81 e2 ff 0f 00 00    	and    edx,0xfff
  102bc8:	09 f2                	or     edx,esi
  102bca:	89 14 85 00 90 11 00 	mov    DWORD PTR [eax*4+0x119000],edx
    for (uint i = 0; i < 256; i++)
  102bd1:	83 c0 01             	add    eax,0x1
  102bd4:	3d 00 01 00 00       	cmp    eax,0x100
  102bd9:	75 bd                	jne    102b98 <memory_setup+0x38>
    uint count = PAGE_ALIGN(used) / PAGE_SIZE;
  102bdb:	81 e3 00 f0 ff ff    	and    ebx,0xfffff000
    log("P_S_USED ADDR=0x%x COUNT=%d", addr, count);
  102be1:	83 ec 04             	sub    esp,0x4
    uint count = PAGE_ALIGN(used) / PAGE_SIZE;
  102be4:	81 c3 00 10 00 00    	add    ebx,0x1000
  102bea:	c1 eb 0c             	shr    ebx,0xc
    log("P_S_USED ADDR=0x%x COUNT=%d", addr, count);
  102bed:	53                   	push   ebx
  102bee:	6a 00                	push   0x0
  102bf0:	68 a7 20 11 00       	push   0x1120a7
  102bf5:	e8 a6 ea ff ff       	call   1016a0 <log>
    for (uint i = 0; i < count; i++)
  102bfa:	83 c4 10             	add    esp,0x10
  102bfd:	85 db                	test   ebx,ebx
  102bff:	74 2f                	je     102c30 <memory_setup+0xd0>
  102c01:	31 c0                	xor    eax,eax
        PHYSICAL_SET_USED(addr + (i * PAGE_SIZE));
  102c03:	be 01 00 00 00       	mov    esi,0x1
  102c08:	90                   	nop
  102c09:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
  102c10:	89 c2                	mov    edx,eax
  102c12:	89 c1                	mov    ecx,eax
  102c14:	89 f7                	mov    edi,esi
  102c16:	83 e1 07             	and    ecx,0x7
  102c19:	c1 e2 0c             	shl    edx,0xc
    for (uint i = 0; i < count; i++)
  102c1c:	83 c0 01             	add    eax,0x1
        PHYSICAL_SET_USED(addr + (i * PAGE_SIZE));
  102c1f:	c1 ea 0f             	shr    edx,0xf
  102c22:	d3 e7                	shl    edi,cl
  102c24:	89 f9                	mov    ecx,edi
  102c26:	08 8a 00 a0 21 00    	or     BYTE PTR [edx+0x21a000],cl
    for (uint i = 0; i < count; i++)
  102c2c:	39 c3                	cmp    ebx,eax
  102c2e:	75 e0                	jne    102c10 <memory_setup+0xb0>
    virtual_map(&kpdir, 0, 0, count, 0);
  102c30:	83 ec 0c             	sub    esp,0xc
  102c33:	6a 00                	push   0x0
  102c35:	53                   	push   ebx
  102c36:	6a 00                	push   0x0
  102c38:	6a 00                	push   0x0
  102c3a:	68 00 90 11 00       	push   0x119000
  102c3f:	e8 2c fc ff ff       	call   102870 <virtual_map>
    log("Enabling paging...");
  102c44:	83 c4 14             	add    esp,0x14
  102c47:	68 15 21 11 00       	push   0x112115
  102c4c:	e8 4f ea ff ff       	call   1016a0 <log>
    paging_load_directorie(&kpdir);
  102c51:	c7 04 24 00 90 11 00 	mov    DWORD PTR [esp],0x119000
  102c58:	e8 bf 27 00 00       	call   10541c <paging_load_directorie>
    paging_enable();
  102c5d:	e8 ae 27 00 00       	call   105410 <paging_enable>
    log("Paging enabled!");
  102c62:	83 c4 10             	add    esp,0x10
  102c65:	c7 44 24 10 28 21 11 	mov    DWORD PTR [esp+0x10],0x112128
  102c6c:	00 
}
  102c6d:	5b                   	pop    ebx
  102c6e:	5e                   	pop    esi
  102c6f:	5f                   	pop    edi
    log("Paging enabled!");
  102c70:	e9 2b ea ff ff       	jmp    1016a0 <log>
  102c75:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
  102c79:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00102c80 <memory_kpdir>:
}
  102c80:	b8 00 90 11 00       	mov    eax,0x119000
  102c85:	c3                   	ret    
  102c86:	8d 76 00             	lea    esi,[esi+0x0]
  102c89:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00102c90 <memory_alloc>:
{
  102c90:	55                   	push   ebp
  102c91:	57                   	push   edi
  102c92:	56                   	push   esi
  102c93:	53                   	push   ebx
  102c94:	83 ec 1c             	sub    esp,0x1c
  102c97:	8b 7c 24 30          	mov    edi,DWORD PTR [esp+0x30]
    atomic_begin();
  102c9b:	e8 a0 51 00 00       	call   107e40 <atomic_begin>
    log("PALLOC COUNT=%d", count);
  102ca0:	83 ec 08             	sub    esp,0x8
  102ca3:	57                   	push   edi
  102ca4:	68 82 20 11 00       	push   0x112082
  102ca9:	e8 f2 e9 ff ff       	call   1016a0 <log>
    for (uint i = 0; i < (TOTAL_MEMORY / PAGE_SIZE); i++)
  102cae:	8b 35 10 ae 23 00    	mov    esi,DWORD PTR ds:0x23ae10
  102cb4:	83 c4 10             	add    esp,0x10
  102cb7:	c1 ee 0c             	shr    esi,0xc
  102cba:	85 f6                	test   esi,esi
  102cbc:	0f 84 ca 00 00 00    	je     102d8c <memory_alloc+0xfc>
  102cc2:	c1 e6 0c             	shl    esi,0xc
  102cc5:	89 74 24 0c          	mov    DWORD PTR [esp+0xc],esi
  102cc9:	31 f6                	xor    esi,esi
  102ccb:	90                   	nop
  102ccc:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
    for (uint i = 0; i < count; i++)
  102cd0:	85 ff                	test   edi,edi
  102cd2:	0f 84 e8 00 00 00    	je     102dc0 <memory_alloc+0x130>
        if (PHYSICAL_IS_USED(addr + i * PAGE_SIZE))
  102cd8:	89 f0                	mov    eax,esi
  102cda:	8d ae 00 10 00 00    	lea    ebp,[esi+0x1000]
  102ce0:	c1 e8 0f             	shr    eax,0xf
  102ce3:	0f b6 90 00 a0 21 00 	movzx  edx,BYTE PTR [eax+0x21a000]
  102cea:	89 f0                	mov    eax,esi
  102cec:	c1 e8 0c             	shr    eax,0xc
  102cef:	83 e0 07             	and    eax,0x7
  102cf2:	0f a3 c2             	bt     edx,eax
  102cf5:	0f 82 a5 00 00 00    	jb     102da0 <memory_alloc+0x110>
  102cfb:	89 e8                	mov    eax,ebp
    for (uint i = 0; i < count; i++)
  102cfd:	31 db                	xor    ebx,ebx
  102cff:	eb 25                	jmp    102d26 <memory_alloc+0x96>
  102d01:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
        if (PHYSICAL_IS_USED(addr + i * PAGE_SIZE))
  102d08:	89 c2                	mov    edx,eax
  102d0a:	c1 ea 0f             	shr    edx,0xf
  102d0d:	0f b6 8a 00 a0 21 00 	movzx  ecx,BYTE PTR [edx+0x21a000]
  102d14:	89 c2                	mov    edx,eax
  102d16:	05 00 10 00 00       	add    eax,0x1000
  102d1b:	c1 ea 0c             	shr    edx,0xc
  102d1e:	83 e2 07             	and    edx,0x7
  102d21:	0f a3 d1             	bt     ecx,edx
  102d24:	72 7a                	jb     102da0 <memory_alloc+0x110>
    for (uint i = 0; i < count; i++)
  102d26:	83 c3 01             	add    ebx,0x1
  102d29:	39 df                	cmp    edi,ebx
  102d2b:	75 db                	jne    102d08 <memory_alloc+0x78>
    log("P_S_USED ADDR=0x%x COUNT=%d", addr, count);
  102d2d:	83 ec 04             	sub    esp,0x4
  102d30:	57                   	push   edi
  102d31:	56                   	push   esi
  102d32:	68 a7 20 11 00       	push   0x1120a7
  102d37:	e8 64 e9 ff ff       	call   1016a0 <log>
  102d3c:	83 c4 10             	add    esp,0x10
  102d3f:	89 f0                	mov    eax,esi
    for (uint i = 0; i < count; i++)
  102d41:	31 d2                	xor    edx,edx
  102d43:	89 74 24 0c          	mov    DWORD PTR [esp+0xc],esi
        PHYSICAL_SET_USED(addr + (i * PAGE_SIZE));
  102d47:	89 c1                	mov    ecx,eax
  102d49:	89 c5                	mov    ebp,eax
  102d4b:	be 01 00 00 00       	mov    esi,0x1
  102d50:	c1 e9 0c             	shr    ecx,0xc
  102d53:	c1 ed 0f             	shr    ebp,0xf
    for (uint i = 0; i < count; i++)
  102d56:	83 c2 01             	add    edx,0x1
        PHYSICAL_SET_USED(addr + (i * PAGE_SIZE));
  102d59:	83 e1 07             	and    ecx,0x7
  102d5c:	05 00 10 00 00       	add    eax,0x1000
  102d61:	d3 e6                	shl    esi,cl
  102d63:	89 f1                	mov    ecx,esi
  102d65:	08 8d 00 a0 21 00    	or     BYTE PTR [ebp+0x21a000],cl
    for (uint i = 0; i < count; i++)
  102d6b:	39 d3                	cmp    ebx,edx
  102d6d:	75 d8                	jne    102d47 <memory_alloc+0xb7>
  102d6f:	8b 74 24 0c          	mov    esi,DWORD PTR [esp+0xc]
    if (addr != 0)
  102d73:	85 f6                	test   esi,esi
  102d75:	74 15                	je     102d8c <memory_alloc+0xfc>
        virtual_map(&kpdir, addr, addr, count, 0);
  102d77:	83 ec 0c             	sub    esp,0xc
  102d7a:	6a 00                	push   0x0
  102d7c:	57                   	push   edi
  102d7d:	56                   	push   esi
  102d7e:	56                   	push   esi
  102d7f:	68 00 90 11 00       	push   0x119000
  102d84:	e8 e7 fa ff ff       	call   102870 <virtual_map>
  102d89:	83 c4 20             	add    esp,0x20
    atomic_end();
  102d8c:	e8 cf 50 00 00       	call   107e60 <atomic_end>

    return addr;
}
  102d91:	83 c4 1c             	add    esp,0x1c
  102d94:	89 f0                	mov    eax,esi
  102d96:	5b                   	pop    ebx
  102d97:	5e                   	pop    esi
  102d98:	5f                   	pop    edi
  102d99:	5d                   	pop    ebp
  102d9a:	c3                   	ret    
  102d9b:	90                   	nop
  102d9c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
    for (uint i = 0; i < (TOTAL_MEMORY / PAGE_SIZE); i++)
  102da0:	39 6c 24 0c          	cmp    DWORD PTR [esp+0xc],ebp
  102da4:	89 ee                	mov    esi,ebp
  102da6:	0f 85 24 ff ff ff    	jne    102cd0 <memory_alloc+0x40>
    atomic_end();
  102dac:	e8 af 50 00 00       	call   107e60 <atomic_end>
    return 0;
  102db1:	31 f6                	xor    esi,esi
}
  102db3:	83 c4 1c             	add    esp,0x1c
  102db6:	5b                   	pop    ebx
  102db7:	89 f0                	mov    eax,esi
  102db9:	5e                   	pop    esi
  102dba:	5f                   	pop    edi
  102dbb:	5d                   	pop    ebp
  102dbc:	c3                   	ret    
  102dbd:	8d 76 00             	lea    esi,[esi+0x0]
    log("P_S_USED ADDR=0x%x COUNT=%d", addr, count);
  102dc0:	83 ec 04             	sub    esp,0x4
  102dc3:	6a 00                	push   0x0
  102dc5:	56                   	push   esi
  102dc6:	68 a7 20 11 00       	push   0x1120a7
  102dcb:	e8 d0 e8 ff ff       	call   1016a0 <log>
  102dd0:	83 c4 10             	add    esp,0x10
  102dd3:	eb 9e                	jmp    102d73 <memory_alloc+0xe3>
  102dd5:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
  102dd9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00102de0 <memory_free>:

void memory_free(uint addr, uint count)
{
  102de0:	55                   	push   ebp
  102de1:	57                   	push   edi
  102de2:	56                   	push   esi
  102de3:	53                   	push   ebx
  102de4:	83 ec 1c             	sub    esp,0x1c
  102de7:	8b 44 24 34          	mov    eax,DWORD PTR [esp+0x34]
  102deb:	8b 5c 24 30          	mov    ebx,DWORD PTR [esp+0x30]
  102def:	89 44 24 0c          	mov    DWORD PTR [esp+0xc],eax
  102df3:	89 c7                	mov    edi,eax
    atomic_begin();
  102df5:	e8 46 50 00 00       	call   107e40 <atomic_begin>
    log("P_S_FREE ADDR=%d COUNT=%d", addr, count);
  102dfa:	83 ec 04             	sub    esp,0x4
  102dfd:	57                   	push   edi
  102dfe:	53                   	push   ebx
  102dff:	68 c3 20 11 00       	push   0x1120c3
  102e04:	e8 97 e8 ff ff       	call   1016a0 <log>
    for (uint i = 0; i < count; i++)
  102e09:	83 c4 10             	add    esp,0x10
  102e0c:	85 ff                	test   edi,edi
  102e0e:	74 7b                	je     102e8b <memory_free+0xab>
  102e10:	89 da                	mov    edx,ebx
  102e12:	31 c0                	xor    eax,eax
        PHYSICAL_SET_FREE(addr + (i * PAGE_SIZE));
  102e14:	bd 01 00 00 00       	mov    ebp,0x1
  102e19:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
  102e20:	89 d1                	mov    ecx,edx
  102e22:	89 ef                	mov    edi,ebp
  102e24:	89 d6                	mov    esi,edx
  102e26:	c1 e9 0c             	shr    ecx,0xc
  102e29:	c1 ee 0f             	shr    esi,0xf
    for (uint i = 0; i < count; i++)
  102e2c:	83 c0 01             	add    eax,0x1
        PHYSICAL_SET_FREE(addr + (i * PAGE_SIZE));
  102e2f:	83 e1 07             	and    ecx,0x7
  102e32:	81 c2 00 10 00 00    	add    edx,0x1000
  102e38:	d3 e7                	shl    edi,cl
  102e3a:	89 f9                	mov    ecx,edi
  102e3c:	f7 d1                	not    ecx
  102e3e:	20 8e 00 a0 21 00    	and    BYTE PTR [esi+0x21a000],cl
    for (uint i = 0; i < count; i++)
  102e44:	39 44 24 0c          	cmp    DWORD PTR [esp+0xc],eax
  102e48:	75 d6                	jne    102e20 <memory_free+0x40>
    for (uint i = 0; i < count; i++)
  102e4a:	31 d2                	xor    edx,edx
  102e4c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
        uint pdi = PD_INDEX(vaddr + offset);
  102e50:	89 d9                	mov    ecx,ebx
  102e52:	c1 e9 16             	shr    ecx,0x16
        if (pde->Present)
  102e55:	f6 04 8d 00 90 11 00 	test   BYTE PTR [ecx*4+0x119000],0x1
  102e5c:	01 
  102e5d:	74 1f                	je     102e7e <memory_free+0x9e>
        page_table_t *ptable = (page_table_t *)(pde->PageFrameNumber * PAGE_SIZE);
  102e5f:	8b 0c 8d 00 90 11 00 	mov    ecx,DWORD PTR [ecx*4+0x119000]
        uint pti = PT_INDEX(vaddr + offset);
  102e66:	89 de                	mov    esi,ebx
  102e68:	c1 ee 0c             	shr    esi,0xc
  102e6b:	81 e6 ff 03 00 00    	and    esi,0x3ff
            p->as_uint = 0;
  102e71:	81 e1 00 f0 ff ff    	and    ecx,0xfffff000
  102e77:	c7 04 b1 00 00 00 00 	mov    DWORD PTR [ecx+esi*4],0x0
    for (uint i = 0; i < count; i++)
  102e7e:	83 c2 01             	add    edx,0x1
  102e81:	81 c3 00 10 00 00    	add    ebx,0x1000
  102e87:	39 c2                	cmp    edx,eax
  102e89:	75 c5                	jne    102e50 <memory_free+0x70>
    paging_invalidate_tlb();
  102e8b:	e8 94 25 00 00       	call   105424 <paging_invalidate_tlb>

    physical_free(addr, count);
    virtual_unmap(&kpdir, addr, count);

    atomic_end();
}
  102e90:	83 c4 1c             	add    esp,0x1c
  102e93:	5b                   	pop    ebx
  102e94:	5e                   	pop    esi
  102e95:	5f                   	pop    edi
  102e96:	5d                   	pop    ebp
    atomic_end();
  102e97:	e9 c4 4f 00 00       	jmp    107e60 <atomic_end>
  102e9c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]

00102ea0 <memory_alloc_pdir>:

// Alloc a pdir for a process
page_directorie_t *memory_alloc_pdir()
{
  102ea0:	57                   	push   edi
  102ea1:	56                   	push   esi
  102ea2:	53                   	push   ebx
    atomic_begin();
  102ea3:	e8 98 4f 00 00       	call   107e40 <atomic_begin>
    atomic_begin();
  102ea8:	e8 93 4f 00 00       	call   107e40 <atomic_begin>
    log("PALLOC COUNT=%d", count);
  102ead:	83 ec 08             	sub    esp,0x8
  102eb0:	6a 01                	push   0x1
  102eb2:	68 82 20 11 00       	push   0x112082
  102eb7:	e8 e4 e7 ff ff       	call   1016a0 <log>
    for (uint i = 0; i < (TOTAL_MEMORY / PAGE_SIZE); i++)
  102ebc:	8b 15 10 ae 23 00    	mov    edx,DWORD PTR ds:0x23ae10
  102ec2:	83 c4 10             	add    esp,0x10
  102ec5:	c1 ea 0c             	shr    edx,0xc
  102ec8:	85 d2                	test   edx,edx
  102eca:	74 24                	je     102ef0 <memory_alloc_pdir+0x50>
        if (PHYSICAL_IS_USED(addr + i * PAGE_SIZE))
  102ecc:	f6 05 00 a0 21 00 01 	test   BYTE PTR ds:0x21a000,0x1
  102ed3:	75 69                	jne    102f3e <memory_alloc_pdir+0x9e>
    log("P_S_USED ADDR=0x%x COUNT=%d", addr, count);
  102ed5:	83 ec 04             	sub    esp,0x4
  102ed8:	6a 01                	push   0x1
  102eda:	6a 00                	push   0x0
  102edc:	68 a7 20 11 00       	push   0x1120a7
  102ee1:	e8 ba e7 ff ff       	call   1016a0 <log>
        PHYSICAL_SET_USED(addr + (i * PAGE_SIZE));
  102ee6:	80 0d 00 a0 21 00 01 	or     BYTE PTR ds:0x21a000,0x1
  102eed:	83 c4 10             	add    esp,0x10
  102ef0:	31 db                	xor    ebx,ebx
    atomic_end();
  102ef2:	e8 69 4f 00 00       	call   107e60 <atomic_end>
  102ef7:	b9 00 a0 11 00       	mov    ecx,0x11a000

    page_directorie_t *pdir = (page_directorie_t *)memory_alloc(1);

    // Copy first gigs of virtual memory (kernel space);
    for (uint i = 0; i < 256; i++)
  102efc:	31 c0                	xor    eax,eax
  102efe:	66 90                	xchg   ax,ax
    {
        page_directorie_entry_t *e = &pdir->entries[i];
        e->User = 0;
  102f00:	0f b6 14 83          	movzx  edx,BYTE PTR [ebx+eax*4]
        e->Write = 1;
        e->Present = 1;
        e->PageFrameNumber = (uint)(&kptable[i]) / PAGE_SIZE;
  102f04:	89 ce                	mov    esi,ecx
  102f06:	81 c1 00 10 00 00    	add    ecx,0x1000
  102f0c:	81 e6 00 f0 ff ff    	and    esi,0xfffff000
        e->User = 0;
  102f12:	83 e2 fb             	and    edx,0xfffffffb
        e->Present = 1;
  102f15:	83 ca 03             	or     edx,0x3
  102f18:	88 14 83             	mov    BYTE PTR [ebx+eax*4],dl
        e->PageFrameNumber = (uint)(&kptable[i]) / PAGE_SIZE;
  102f1b:	8b 14 83             	mov    edx,DWORD PTR [ebx+eax*4]
  102f1e:	81 e2 ff 0f 00 00    	and    edx,0xfff
  102f24:	09 f2                	or     edx,esi
  102f26:	89 14 83             	mov    DWORD PTR [ebx+eax*4],edx
    for (uint i = 0; i < 256; i++)
  102f29:	83 c0 01             	add    eax,0x1
  102f2c:	3d 00 01 00 00       	cmp    eax,0x100
  102f31:	75 cd                	jne    102f00 <memory_alloc_pdir+0x60>
    }

    atomic_end();
  102f33:	e8 28 4f 00 00       	call   107e60 <atomic_end>

    return pdir;
}
  102f38:	89 d8                	mov    eax,ebx
  102f3a:	5b                   	pop    ebx
  102f3b:	5e                   	pop    esi
  102f3c:	5f                   	pop    edi
  102f3d:	c3                   	ret    
  102f3e:	c1 e2 0c             	shl    edx,0xc
        if (PHYSICAL_IS_USED(addr + i * PAGE_SIZE))
  102f41:	bb 00 10 00 00       	mov    ebx,0x1000
    for (uint i = 0; i < (TOTAL_MEMORY / PAGE_SIZE); i++)
  102f46:	39 da                	cmp    edx,ebx
  102f48:	74 a6                	je     102ef0 <memory_alloc_pdir+0x50>
        if (PHYSICAL_IS_USED(addr + i * PAGE_SIZE))
  102f4a:	89 df                	mov    edi,ebx
  102f4c:	89 de                	mov    esi,ebx
  102f4e:	8d 8b 00 10 00 00    	lea    ecx,[ebx+0x1000]
  102f54:	c1 ef 0f             	shr    edi,0xf
  102f57:	c1 ee 0c             	shr    esi,0xc
  102f5a:	0f b6 87 00 a0 21 00 	movzx  eax,BYTE PTR [edi+0x21a000]
  102f61:	83 e6 07             	and    esi,0x7
  102f64:	0f a3 f0             	bt     eax,esi
  102f67:	72 45                	jb     102fae <memory_alloc_pdir+0x10e>
    log("P_S_USED ADDR=0x%x COUNT=%d", addr, count);
  102f69:	83 ec 04             	sub    esp,0x4
  102f6c:	6a 01                	push   0x1
  102f6e:	53                   	push   ebx
  102f6f:	68 a7 20 11 00       	push   0x1120a7
  102f74:	e8 27 e7 ff ff       	call   1016a0 <log>
        PHYSICAL_SET_USED(addr + (i * PAGE_SIZE));
  102f79:	b8 01 00 00 00       	mov    eax,0x1
  102f7e:	89 f1                	mov    ecx,esi
    if (addr != 0)
  102f80:	83 c4 10             	add    esp,0x10
        PHYSICAL_SET_USED(addr + (i * PAGE_SIZE));
  102f83:	d3 e0                	shl    eax,cl
  102f85:	08 87 00 a0 21 00    	or     BYTE PTR [edi+0x21a000],al
    if (addr != 0)
  102f8b:	85 db                	test   ebx,ebx
  102f8d:	0f 84 5f ff ff ff    	je     102ef2 <memory_alloc_pdir+0x52>
        virtual_map(&kpdir, addr, addr, count, 0);
  102f93:	83 ec 0c             	sub    esp,0xc
  102f96:	89 d9                	mov    ecx,ebx
  102f98:	89 da                	mov    edx,ebx
  102f9a:	6a 00                	push   0x0
  102f9c:	b8 00 90 11 00       	mov    eax,0x119000
  102fa1:	e8 da f1 ff ff       	call   102180 <virtual_map.constprop.2>
  102fa6:	83 c4 10             	add    esp,0x10
  102fa9:	e9 44 ff ff ff       	jmp    102ef2 <memory_alloc_pdir+0x52>
        if (PHYSICAL_IS_USED(addr + i * PAGE_SIZE))
  102fae:	89 cb                	mov    ebx,ecx
  102fb0:	eb 94                	jmp    102f46 <memory_alloc_pdir+0xa6>
  102fb2:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
  102fb9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00102fc0 <memory_free_pdir>:

// Free the pdir of a dying process
void memory_free_pdir(page_directorie_t *pdir)
{
  102fc0:	55                   	push   ebp
  102fc1:	57                   	push   edi
  102fc2:	56                   	push   esi
  102fc3:	53                   	push   ebx
        PHYSICAL_SET_FREE(addr + (i * PAGE_SIZE));
  102fc4:	be 01 00 00 00       	mov    esi,0x1
{
  102fc9:	83 ec 1c             	sub    esp,0x1c
  102fcc:	8b 44 24 30          	mov    eax,DWORD PTR [esp+0x30]
  102fd0:	89 44 24 08          	mov    DWORD PTR [esp+0x8],eax
    atomic_begin();
  102fd4:	e8 67 4e 00 00       	call   107e40 <atomic_begin>

    for (size_t i = 256; i < 1024; i++)
  102fd9:	c7 44 24 04 00 01 00 	mov    DWORD PTR [esp+0x4],0x100
  102fe0:	00 
  102fe1:	eb 1d                	jmp    103000 <memory_free_pdir+0x40>
  102fe3:	83 44 24 04 01       	add    DWORD PTR [esp+0x4],0x1
  102fe8:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  102fec:	3d 00 04 00 00       	cmp    eax,0x400
  102ff1:	0f 84 19 01 00 00    	je     103110 <memory_free_pdir+0x150>
  102ff7:	89 f6                	mov    esi,esi
  102ff9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
    {
        page_directorie_entry_t *e = &pdir->entries[i];

        if (e->Present)
  103000:	8b 44 24 08          	mov    eax,DWORD PTR [esp+0x8]
  103004:	8b 54 24 04          	mov    edx,DWORD PTR [esp+0x4]
  103008:	f6 04 90 01          	test   BYTE PTR [eax+edx*4],0x1
  10300c:	74 d5                	je     102fe3 <memory_free_pdir+0x23>
        {
            page_table_t *pt = (page_table_t *)(e->PageFrameNumber * PAGE_SIZE);
  10300e:	89 c1                	mov    ecx,eax
  103010:	0f b6 44 90 01       	movzx  eax,BYTE PTR [eax+edx*4+0x1]
  103015:	89 d7                	mov    edi,edx
            for (size_t i = 0; i < 1024; i++)
  103017:	31 db                	xor    ebx,ebx
            page_table_t *pt = (page_table_t *)(e->PageFrameNumber * PAGE_SIZE);
  103019:	88 44 24 0f          	mov    BYTE PTR [esp+0xf],al
  10301d:	89 c2                	mov    edx,eax
  10301f:	0f b6 44 b9 02       	movzx  eax,BYTE PTR [ecx+edi*4+0x2]
  103024:	c0 ea 04             	shr    dl,0x4
  103027:	0f b6 d2             	movzx  edx,dl
  10302a:	c1 e0 04             	shl    eax,0x4
  10302d:	09 c2                	or     edx,eax
  10302f:	0f b6 44 b9 03       	movzx  eax,BYTE PTR [ecx+edi*4+0x3]
  103034:	c1 e0 0c             	shl    eax,0xc
  103037:	09 d0                	or     eax,edx
  103039:	c1 e0 0c             	shl    eax,0xc
  10303c:	89 c5                	mov    ebp,eax
  10303e:	eb 0b                	jmp    10304b <memory_free_pdir+0x8b>
            for (size_t i = 0; i < 1024; i++)
  103040:	83 c3 01             	add    ebx,0x1
  103043:	81 fb 00 04 00 00    	cmp    ebx,0x400
  103049:	74 4a                	je     103095 <memory_free_pdir+0xd5>
            {
                page_t *p = &pt->pages[i];

                if (p->Present)
  10304b:	f6 44 9d 00 01       	test   BYTE PTR [ebp+ebx*4+0x0],0x1
  103050:	74 ee                	je     103040 <memory_free_pdir+0x80>
                {
                    physical_free(p->PageFrameNumber * PAGE_SIZE, 1);
  103052:	8b 7c 9d 00          	mov    edi,DWORD PTR [ebp+ebx*4+0x0]
    log("P_S_FREE ADDR=%d COUNT=%d", addr, count);
  103056:	83 ec 04             	sub    esp,0x4
            for (size_t i = 0; i < 1024; i++)
  103059:	83 c3 01             	add    ebx,0x1
    log("P_S_FREE ADDR=%d COUNT=%d", addr, count);
  10305c:	6a 01                	push   0x1
                    physical_free(p->PageFrameNumber * PAGE_SIZE, 1);
  10305e:	81 e7 00 f0 ff ff    	and    edi,0xfffff000
    log("P_S_FREE ADDR=%d COUNT=%d", addr, count);
  103064:	57                   	push   edi
  103065:	68 c3 20 11 00       	push   0x1120c3
  10306a:	e8 31 e6 ff ff       	call   1016a0 <log>
        PHYSICAL_SET_FREE(addr + (i * PAGE_SIZE));
  10306f:	89 fa                	mov    edx,edi
  103071:	c1 ef 0c             	shr    edi,0xc
  103074:	89 f0                	mov    eax,esi
  103076:	89 f9                	mov    ecx,edi
  103078:	c1 ea 0f             	shr    edx,0xf
  10307b:	83 c4 10             	add    esp,0x10
  10307e:	83 e1 07             	and    ecx,0x7
  103081:	d3 e0                	shl    eax,cl
  103083:	89 c1                	mov    ecx,eax
  103085:	f7 d1                	not    ecx
  103087:	20 8a 00 a0 21 00    	and    BYTE PTR [edx+0x21a000],cl
            for (size_t i = 0; i < 1024; i++)
  10308d:	81 fb 00 04 00 00    	cmp    ebx,0x400
  103093:	75 b6                	jne    10304b <memory_free_pdir+0x8b>
    atomic_begin();
  103095:	e8 a6 4d 00 00       	call   107e40 <atomic_begin>
    log("P_S_FREE ADDR=%d COUNT=%d", addr, count);
  10309a:	83 ec 04             	sub    esp,0x4
        PHYSICAL_SET_FREE(addr + (i * PAGE_SIZE));
  10309d:	89 eb                	mov    ebx,ebp
    log("P_S_FREE ADDR=%d COUNT=%d", addr, count);
  10309f:	6a 01                	push   0x1
  1030a1:	55                   	push   ebp
        PHYSICAL_SET_FREE(addr + (i * PAGE_SIZE));
  1030a2:	c1 eb 0f             	shr    ebx,0xf
    log("P_S_FREE ADDR=%d COUNT=%d", addr, count);
  1030a5:	68 c3 20 11 00       	push   0x1120c3
  1030aa:	e8 f1 e5 ff ff       	call   1016a0 <log>
        PHYSICAL_SET_FREE(addr + (i * PAGE_SIZE));
  1030af:	89 ea                	mov    edx,ebp
  1030b1:	89 f0                	mov    eax,esi
        if (pde->Present)
  1030b3:	83 c4 10             	add    esp,0x10
        PHYSICAL_SET_FREE(addr + (i * PAGE_SIZE));
  1030b6:	c1 ea 0c             	shr    edx,0xc
  1030b9:	89 d1                	mov    ecx,edx
  1030bb:	83 e1 07             	and    ecx,0x7
  1030be:	d3 e0                	shl    eax,cl
  1030c0:	89 c1                	mov    ecx,eax
        uint pdi = PD_INDEX(vaddr + offset);
  1030c2:	89 e8                	mov    eax,ebp
  1030c4:	c1 e8 16             	shr    eax,0x16
        PHYSICAL_SET_FREE(addr + (i * PAGE_SIZE));
  1030c7:	f7 d1                	not    ecx
  1030c9:	20 8b 00 a0 21 00    	and    BYTE PTR [ebx+0x21a000],cl
        if (pde->Present)
  1030cf:	f6 04 85 00 90 11 00 	test   BYTE PTR [eax*4+0x119000],0x1
  1030d6:	01 
  1030d7:	74 19                	je     1030f2 <memory_free_pdir+0x132>
        page_table_t *ptable = (page_table_t *)(pde->PageFrameNumber * PAGE_SIZE);
  1030d9:	8b 04 85 00 90 11 00 	mov    eax,DWORD PTR [eax*4+0x119000]
        uint pti = PT_INDEX(vaddr + offset);
  1030e0:	81 e2 ff 03 00 00    	and    edx,0x3ff
            p->as_uint = 0;
  1030e6:	25 00 f0 ff ff       	and    eax,0xfffff000
  1030eb:	c7 04 90 00 00 00 00 	mov    DWORD PTR [eax+edx*4],0x0
    paging_invalidate_tlb();
  1030f2:	e8 2d 23 00 00       	call   105424 <paging_invalidate_tlb>
    atomic_end();
  1030f7:	e8 64 4d 00 00       	call   107e60 <atomic_end>
    for (size_t i = 256; i < 1024; i++)
  1030fc:	83 44 24 04 01       	add    DWORD PTR [esp+0x4],0x1
  103101:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  103105:	3d 00 04 00 00       	cmp    eax,0x400
  10310a:	0f 85 f0 fe ff ff    	jne    103000 <memory_free_pdir+0x40>
    atomic_begin();
  103110:	e8 2b 4d 00 00       	call   107e40 <atomic_begin>
    log("P_S_FREE ADDR=%d COUNT=%d", addr, count);
  103115:	83 ec 04             	sub    esp,0x4
  103118:	6a 01                	push   0x1
  10311a:	8b 74 24 10          	mov    esi,DWORD PTR [esp+0x10]
  10311e:	56                   	push   esi
  10311f:	68 c3 20 11 00       	push   0x1120c3
  103124:	e8 77 e5 ff ff       	call   1016a0 <log>
        PHYSICAL_SET_FREE(addr + (i * PAGE_SIZE));
  103129:	89 f0                	mov    eax,esi
  10312b:	c1 ee 0f             	shr    esi,0xf
        if (pde->Present)
  10312e:	83 c4 10             	add    esp,0x10
        PHYSICAL_SET_FREE(addr + (i * PAGE_SIZE));
  103131:	89 c2                	mov    edx,eax
  103133:	89 f3                	mov    ebx,esi
  103135:	89 c6                	mov    esi,eax
  103137:	c1 ea 0c             	shr    edx,0xc
  10313a:	b8 01 00 00 00       	mov    eax,0x1
        uint pdi = PD_INDEX(vaddr + offset);
  10313f:	c1 ee 16             	shr    esi,0x16
        PHYSICAL_SET_FREE(addr + (i * PAGE_SIZE));
  103142:	89 d1                	mov    ecx,edx
  103144:	83 e1 07             	and    ecx,0x7
  103147:	d3 e0                	shl    eax,cl
  103149:	f7 d0                	not    eax
  10314b:	20 83 00 a0 21 00    	and    BYTE PTR [ebx+0x21a000],al
        if (pde->Present)
  103151:	f6 04 b5 00 90 11 00 	test   BYTE PTR [esi*4+0x119000],0x1
  103158:	01 
  103159:	74 19                	je     103174 <memory_free_pdir+0x1b4>
        page_table_t *ptable = (page_table_t *)(pde->PageFrameNumber * PAGE_SIZE);
  10315b:	8b 04 b5 00 90 11 00 	mov    eax,DWORD PTR [esi*4+0x119000]
        uint pti = PT_INDEX(vaddr + offset);
  103162:	81 e2 ff 03 00 00    	and    edx,0x3ff
            p->as_uint = 0;
  103168:	25 00 f0 ff ff       	and    eax,0xfffff000
  10316d:	c7 04 90 00 00 00 00 	mov    DWORD PTR [eax+edx*4],0x0
    paging_invalidate_tlb();
  103174:	e8 ab 22 00 00       	call   105424 <paging_invalidate_tlb>
    atomic_end();
  103179:	e8 e2 4c 00 00       	call   107e60 <atomic_end>
    }

    memory_free((uint)pdir, 1);

    atomic_end();
}
  10317e:	83 c4 1c             	add    esp,0x1c
  103181:	5b                   	pop    ebx
  103182:	5e                   	pop    esi
  103183:	5f                   	pop    edi
  103184:	5d                   	pop    ebp
    atomic_end();
  103185:	e9 d6 4c 00 00       	jmp    107e60 <atomic_end>
  10318a:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]

00103190 <memory_map>:

int memory_map(page_directorie_t *pdir, uint addr, uint count, int user)
{
  103190:	55                   	push   ebp
  103191:	57                   	push   edi
  103192:	56                   	push   esi
  103193:	53                   	push   ebx
  103194:	83 ec 1c             	sub    esp,0x1c
  103197:	8b 5c 24 30          	mov    ebx,DWORD PTR [esp+0x30]
  10319b:	8b 6c 24 34          	mov    ebp,DWORD PTR [esp+0x34]
    atomic_begin();
  10319f:	e8 9c 4c 00 00       	call   107e40 <atomic_begin>

    log("MAP: PDIR=0x%x ADDR=0x%x COUNT=%d",pdir, addr, count);
  1031a4:	ff 74 24 38          	push   DWORD PTR [esp+0x38]
  1031a8:	55                   	push   ebp
  1031a9:	53                   	push   ebx
  1031aa:	68 04 66 11 00       	push   0x116604
  1031af:	e8 ec e4 ff ff       	call   1016a0 <log>

    for (uint i = 0; i < count; i++)
  1031b4:	83 c4 10             	add    esp,0x10
  1031b7:	8b 44 24 38          	mov    eax,DWORD PTR [esp+0x38]
  1031bb:	85 c0                	test   eax,eax
  1031bd:	0f 84 0d 01 00 00    	je     1032d0 <memory_map+0x140>
        uint vaddr = addr + i * PAGE_SIZE;

        if (!virtual_present(pdir, vaddr, 1))
        {
            uint paddr = physical_alloc(1);
            virtual_map(pdir, vaddr, paddr, 1, user);
  1031c3:	0f b6 44 24 3c       	movzx  eax,BYTE PTR [esp+0x3c]
    for (uint i = 0; i < count; i++)
  1031c8:	c7 44 24 04 00 00 00 	mov    DWORD PTR [esp+0x4],0x0
  1031cf:	00 
            virtual_map(pdir, vaddr, paddr, 1, user);
  1031d0:	89 44 24 0c          	mov    DWORD PTR [esp+0xc],eax
  1031d4:	eb 34                	jmp    10320a <memory_map+0x7a>
  1031d6:	8d 76 00             	lea    esi,[esi+0x0]
  1031d9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
    log("0x%x present!", vaddr);
  1031e0:	83 ec 08             	sub    esp,0x8
  1031e3:	55                   	push   ebp
  1031e4:	68 ea 20 11 00       	push   0x1120ea
  1031e9:	e8 b2 e4 ff ff       	call   1016a0 <log>
  1031ee:	83 c4 10             	add    esp,0x10
    for (uint i = 0; i < count; i++)
  1031f1:	83 44 24 04 01       	add    DWORD PTR [esp+0x4],0x1
  1031f6:	81 c5 00 10 00 00    	add    ebp,0x1000
  1031fc:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  103200:	39 44 24 38          	cmp    DWORD PTR [esp+0x38],eax
  103204:	0f 84 c6 00 00 00    	je     1032d0 <memory_map+0x140>
        uint pdi = PD_INDEX(vaddr + offset);
  10320a:	89 e9                	mov    ecx,ebp
        uint pti = PT_INDEX(vaddr + offset);
  10320c:	89 ee                	mov    esi,ebp
  10320e:	c1 ee 0c             	shr    esi,0xc
        uint pdi = PD_INDEX(vaddr + offset);
  103211:	c1 e9 16             	shr    ecx,0x16
        uint pti = PT_INDEX(vaddr + offset);
  103214:	81 e6 ff 03 00 00    	and    esi,0x3ff
        if (!pde->Present)
  10321a:	f6 04 8b 01          	test   BYTE PTR [ebx+ecx*4],0x1
  10321e:	74 28                	je     103248 <memory_map+0xb8>
        page_table_t *ptable = (page_table_t *)(pde->PageFrameNumber * PAGE_SIZE);
  103220:	0f b6 54 8b 01       	movzx  edx,BYTE PTR [ebx+ecx*4+0x1]
  103225:	0f b6 44 8b 02       	movzx  eax,BYTE PTR [ebx+ecx*4+0x2]
  10322a:	c0 ea 04             	shr    dl,0x4
  10322d:	c1 e0 04             	shl    eax,0x4
  103230:	0f b6 d2             	movzx  edx,dl
  103233:	09 c2                	or     edx,eax
  103235:	0f b6 44 8b 03       	movzx  eax,BYTE PTR [ebx+ecx*4+0x3]
  10323a:	c1 e0 0c             	shl    eax,0xc
  10323d:	09 d0                	or     eax,edx
        if (!p->Present)
  10323f:	c1 e0 0c             	shl    eax,0xc
  103242:	f6 04 b0 01          	test   BYTE PTR [eax+esi*4],0x1
  103246:	75 98                	jne    1031e0 <memory_map+0x50>
            log("0x%x absent!", vaddr);
  103248:	83 ec 08             	sub    esp,0x8
  10324b:	55                   	push   ebp
  10324c:	68 dd 20 11 00       	push   0x1120dd
  103251:	e8 4a e4 ff ff       	call   1016a0 <log>
  103256:	83 c4 10             	add    esp,0x10
    log("PALLOC COUNT=%d", count);
  103259:	83 ec 08             	sub    esp,0x8
  10325c:	6a 01                	push   0x1
  10325e:	68 82 20 11 00       	push   0x112082
  103263:	e8 38 e4 ff ff       	call   1016a0 <log>
    for (uint i = 0; i < (TOTAL_MEMORY / PAGE_SIZE); i++)
  103268:	8b 35 10 ae 23 00    	mov    esi,DWORD PTR ds:0x23ae10
  10326e:	83 c4 10             	add    esp,0x10
  103271:	c1 ee 0c             	shr    esi,0xc
  103274:	85 f6                	test   esi,esi
  103276:	74 3a                	je     1032b2 <memory_map+0x122>
        if (PHYSICAL_IS_USED(addr + i * PAGE_SIZE))
  103278:	f6 05 00 a0 21 00 01 	test   BYTE PTR ds:0x21a000,0x1
  10327f:	75 5e                	jne    1032df <memory_map+0x14f>
  103281:	b8 01 00 00 00       	mov    eax,0x1
  103286:	31 ff                	xor    edi,edi
        uint addr = i * PAGE_SIZE;
  103288:	31 f6                	xor    esi,esi
  10328a:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
  103290:	88 44 24 08          	mov    BYTE PTR [esp+0x8],al
    log("P_S_USED ADDR=0x%x COUNT=%d", addr, count);
  103294:	83 ec 04             	sub    esp,0x4
  103297:	6a 01                	push   0x1
  103299:	56                   	push   esi
  10329a:	68 a7 20 11 00       	push   0x1120a7
  10329f:	e8 fc e3 ff ff       	call   1016a0 <log>
        PHYSICAL_SET_USED(addr + (i * PAGE_SIZE));
  1032a4:	0f b6 44 24 18       	movzx  eax,BYTE PTR [esp+0x18]
  1032a9:	83 c4 10             	add    esp,0x10
  1032ac:	08 87 00 a0 21 00    	or     BYTE PTR [edi+0x21a000],al
            virtual_map(pdir, vaddr, paddr, 1, user);
  1032b2:	83 ec 0c             	sub    esp,0xc
  1032b5:	89 f1                	mov    ecx,esi
  1032b7:	89 ea                	mov    edx,ebp
  1032b9:	ff 74 24 18          	push   DWORD PTR [esp+0x18]
  1032bd:	89 d8                	mov    eax,ebx
  1032bf:	e8 bc ee ff ff       	call   102180 <virtual_map.constprop.2>
  1032c4:	83 c4 10             	add    esp,0x10
  1032c7:	e9 25 ff ff ff       	jmp    1031f1 <memory_map+0x61>
  1032cc:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
        }
    }

    atomic_end();
  1032d0:	e8 8b 4b 00 00       	call   107e60 <atomic_end>

    return 0;
}
  1032d5:	83 c4 1c             	add    esp,0x1c
  1032d8:	31 c0                	xor    eax,eax
  1032da:	5b                   	pop    ebx
  1032db:	5e                   	pop    esi
  1032dc:	5f                   	pop    edi
  1032dd:	5d                   	pop    ebp
  1032de:	c3                   	ret    
  1032df:	c1 e6 0c             	shl    esi,0xc
  1032e2:	89 74 24 08          	mov    DWORD PTR [esp+0x8],esi
        if (PHYSICAL_IS_USED(addr + i * PAGE_SIZE))
  1032e6:	be 00 10 00 00       	mov    esi,0x1000
  1032eb:	90                   	nop
  1032ec:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
    for (uint i = 0; i < (TOTAL_MEMORY / PAGE_SIZE); i++)
  1032f0:	39 74 24 08          	cmp    DWORD PTR [esp+0x8],esi
  1032f4:	74 32                	je     103328 <memory_map+0x198>
        if (PHYSICAL_IS_USED(addr + i * PAGE_SIZE))
  1032f6:	89 f7                	mov    edi,esi
  1032f8:	89 f1                	mov    ecx,esi
  1032fa:	8d 96 00 10 00 00    	lea    edx,[esi+0x1000]
  103300:	c1 ef 0f             	shr    edi,0xf
  103303:	c1 e9 0c             	shr    ecx,0xc
  103306:	0f b6 87 00 a0 21 00 	movzx  eax,BYTE PTR [edi+0x21a000]
  10330d:	83 e1 07             	and    ecx,0x7
  103310:	0f a3 c8             	bt     eax,ecx
  103313:	72 1b                	jb     103330 <memory_map+0x1a0>
  103315:	b8 01 00 00 00       	mov    eax,0x1
  10331a:	d3 e0                	shl    eax,cl
  10331c:	e9 6f ff ff ff       	jmp    103290 <memory_map+0x100>
  103321:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
    return 0;
  103328:	31 f6                	xor    esi,esi
  10332a:	eb 86                	jmp    1032b2 <memory_map+0x122>
  10332c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
        if (PHYSICAL_IS_USED(addr + i * PAGE_SIZE))
  103330:	89 d6                	mov    esi,edx
  103332:	eb bc                	jmp    1032f0 <memory_map+0x160>
  103334:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
  10333a:	8d bf 00 00 00 00    	lea    edi,[edi+0x0]

00103340 <memory_unmap>:

int memory_unmap(page_directorie_t *pdir, uint addr, uint count)
{
  103340:	55                   	push   ebp
  103341:	57                   	push   edi
  103342:	56                   	push   esi
  103343:	53                   	push   ebx
  103344:	83 ec 1c             	sub    esp,0x1c
  103347:	8b 6c 24 30          	mov    ebp,DWORD PTR [esp+0x30]
  10334b:	8b 74 24 34          	mov    esi,DWORD PTR [esp+0x34]
    atomic_begin();
  10334f:	e8 ec 4a 00 00       	call   107e40 <atomic_begin>

    for (uint i = 0; i < count; i++)
  103354:	8b 44 24 38          	mov    eax,DWORD PTR [esp+0x38]
  103358:	85 c0                	test   eax,eax
  10335a:	0f 84 30 01 00 00    	je     103490 <memory_unmap+0x150>
  103360:	c7 44 24 0c 00 00 00 	mov    DWORD PTR [esp+0xc],0x0
  103367:	00 
  103368:	e9 c9 00 00 00       	jmp    103436 <memory_unmap+0xf6>
  10336d:	8d 76 00             	lea    esi,[esi+0x0]
    log("0x%x present!", vaddr);
  103370:	83 ec 08             	sub    esp,0x8
  103373:	56                   	push   esi
  103374:	68 ea 20 11 00       	push   0x1120ea
  103379:	e8 22 e3 ff ff       	call   1016a0 <log>
    page_table_t *ptable = (page_table_t *)(pde->PageFrameNumber * PAGE_SIZE);
  10337e:	0f b6 44 9d 01       	movzx  eax,BYTE PTR [ebp+ebx*4+0x1]
  103383:	0f b6 4c 9d 02       	movzx  ecx,BYTE PTR [ebp+ebx*4+0x2]
    log("P_S_FREE ADDR=%d COUNT=%d", addr, count);
  103388:	83 c4 0c             	add    esp,0xc
    page_table_t *ptable = (page_table_t *)(pde->PageFrameNumber * PAGE_SIZE);
  10338b:	c0 e8 04             	shr    al,0x4
  10338e:	c1 e1 04             	shl    ecx,0x4
  103391:	0f b6 c0             	movzx  eax,al
  103394:	09 c1                	or     ecx,eax
  103396:	0f b6 44 9d 03       	movzx  eax,BYTE PTR [ebp+ebx*4+0x3]
  10339b:	c1 e0 0c             	shl    eax,0xc
  10339e:	09 c8                	or     eax,ecx
    return ((p->PageFrameNumber & ~0xfff) + (vaddr & 0xfff));
  1033a0:	c1 e0 0c             	shl    eax,0xc
  1033a3:	8b 0c b8             	mov    ecx,DWORD PTR [eax+edi*4]
  1033a6:	89 f0                	mov    eax,esi
    log("P_S_FREE ADDR=%d COUNT=%d", addr, count);
  1033a8:	6a 01                	push   0x1
    return ((p->PageFrameNumber & ~0xfff) + (vaddr & 0xfff));
  1033aa:	25 ff 0f 00 00       	and    eax,0xfff
  1033af:	c1 e9 0c             	shr    ecx,0xc
  1033b2:	81 e1 00 f0 ff ff    	and    ecx,0xfffff000
  1033b8:	09 c1                	or     ecx,eax
    log("P_S_FREE ADDR=%d COUNT=%d", addr, count);
  1033ba:	51                   	push   ecx
  1033bb:	89 4c 24 14          	mov    DWORD PTR [esp+0x14],ecx
  1033bf:	68 c3 20 11 00       	push   0x1120c3
  1033c4:	e8 d7 e2 ff ff       	call   1016a0 <log>
        PHYSICAL_SET_FREE(addr + (i * PAGE_SIZE));
  1033c9:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
  1033cd:	ba 01 00 00 00       	mov    edx,0x1
        if (pde->Present)
  1033d2:	83 c4 10             	add    esp,0x10
        PHYSICAL_SET_FREE(addr + (i * PAGE_SIZE));
  1033d5:	89 c8                	mov    eax,ecx
  1033d7:	c1 e9 0c             	shr    ecx,0xc
  1033da:	83 e1 07             	and    ecx,0x7
  1033dd:	c1 e8 0f             	shr    eax,0xf
  1033e0:	d3 e2                	shl    edx,cl
  1033e2:	89 d1                	mov    ecx,edx
  1033e4:	f7 d1                	not    ecx
  1033e6:	20 88 00 a0 21 00    	and    BYTE PTR [eax+0x21a000],cl
        if (pde->Present)
  1033ec:	f6 44 9d 00 01       	test   BYTE PTR [ebp+ebx*4+0x0],0x1
  1033f1:	74 29                	je     10341c <memory_unmap+0xdc>
        page_table_t *ptable = (page_table_t *)(pde->PageFrameNumber * PAGE_SIZE);
  1033f3:	0f b6 4c 9d 01       	movzx  ecx,BYTE PTR [ebp+ebx*4+0x1]
  1033f8:	0f b6 44 9d 02       	movzx  eax,BYTE PTR [ebp+ebx*4+0x2]
  1033fd:	c0 e9 04             	shr    cl,0x4
  103400:	c1 e0 04             	shl    eax,0x4
  103403:	0f b6 c9             	movzx  ecx,cl
  103406:	09 c1                	or     ecx,eax
  103408:	0f b6 44 9d 03       	movzx  eax,BYTE PTR [ebp+ebx*4+0x3]
  10340d:	c1 e0 0c             	shl    eax,0xc
  103410:	09 c8                	or     eax,ecx
            p->as_uint = 0;
  103412:	c1 e0 0c             	shl    eax,0xc
  103415:	c7 04 b8 00 00 00 00 	mov    DWORD PTR [eax+edi*4],0x0
    paging_invalidate_tlb();
  10341c:	e8 03 20 00 00       	call   105424 <paging_invalidate_tlb>
    for (uint i = 0; i < count; i++)
  103421:	83 44 24 0c 01       	add    DWORD PTR [esp+0xc],0x1
  103426:	81 c6 00 10 00 00    	add    esi,0x1000
  10342c:	8b 44 24 0c          	mov    eax,DWORD PTR [esp+0xc]
  103430:	39 44 24 38          	cmp    DWORD PTR [esp+0x38],eax
  103434:	74 5a                	je     103490 <memory_unmap+0x150>
        uint pti = PT_INDEX(vaddr + offset);
  103436:	89 f2                	mov    edx,esi
        uint pdi = PD_INDEX(vaddr + offset);
  103438:	89 f3                	mov    ebx,esi
        uint pti = PT_INDEX(vaddr + offset);
  10343a:	c1 ea 0c             	shr    edx,0xc
        uint pdi = PD_INDEX(vaddr + offset);
  10343d:	c1 eb 16             	shr    ebx,0x16
        uint pti = PT_INDEX(vaddr + offset);
  103440:	89 d7                	mov    edi,edx
  103442:	81 e7 ff 03 00 00    	and    edi,0x3ff
        if (!pde->Present)
  103448:	f6 44 9d 00 01       	test   BYTE PTR [ebp+ebx*4+0x0],0x1
  10344d:	74 2c                	je     10347b <memory_unmap+0x13b>
        page_table_t *ptable = (page_table_t *)(pde->PageFrameNumber * PAGE_SIZE);
  10344f:	0f b6 44 9d 01       	movzx  eax,BYTE PTR [ebp+ebx*4+0x1]
  103454:	0f b6 4c 9d 02       	movzx  ecx,BYTE PTR [ebp+ebx*4+0x2]
  103459:	c0 e8 04             	shr    al,0x4
  10345c:	c1 e1 04             	shl    ecx,0x4
  10345f:	0f b6 c0             	movzx  eax,al
  103462:	09 c1                	or     ecx,eax
  103464:	0f b6 44 9d 03       	movzx  eax,BYTE PTR [ebp+ebx*4+0x3]
  103469:	c1 e0 0c             	shl    eax,0xc
  10346c:	09 c8                	or     eax,ecx
        if (!p->Present)
  10346e:	c1 e0 0c             	shl    eax,0xc
  103471:	f6 04 b8 01          	test   BYTE PTR [eax+edi*4],0x1
  103475:	0f 85 f5 fe ff ff    	jne    103370 <memory_unmap+0x30>
            log("0x%x absent!", vaddr);
  10347b:	83 ec 08             	sub    esp,0x8
  10347e:	56                   	push   esi
  10347f:	68 dd 20 11 00       	push   0x1120dd
  103484:	e8 17 e2 ff ff       	call   1016a0 <log>
  103489:	83 c4 10             	add    esp,0x10
  10348c:	eb 93                	jmp    103421 <memory_unmap+0xe1>
  10348e:	66 90                	xchg   ax,ax
            physical_free(virtual2physical(pdir, vaddr), 1);
            virtual_unmap(pdir, vaddr, 1);
        }
    }

    atomic_end();
  103490:	e8 cb 49 00 00       	call   107e60 <atomic_end>

    return 0;
}
  103495:	83 c4 1c             	add    esp,0x1c
  103498:	31 c0                	xor    eax,eax
  10349a:	5b                   	pop    ebx
  10349b:	5e                   	pop    esi
  10349c:	5f                   	pop    edi
  10349d:	5d                   	pop    ebp
  10349e:	c3                   	ret    
  10349f:	90                   	nop

001034a0 <memory_dump>:

void memory_dump(page_directorie_t * pdir)
{
  1034a0:	55                   	push   ebp
  1034a1:	57                   	push   edi
  1034a2:	56                   	push   esi
  1034a3:	53                   	push   ebx
    for(size_t i = 0; i < 1024; i++)
  1034a4:	31 f6                	xor    esi,esi
{
  1034a6:	83 ec 0c             	sub    esp,0xc
  1034a9:	8b 7c 24 20          	mov    edi,DWORD PTR [esp+0x20]
  1034ad:	eb 10                	jmp    1034bf <memory_dump+0x1f>
  1034af:	90                   	nop
    for(size_t i = 0; i < 1024; i++)
  1034b0:	83 c6 01             	add    esi,0x1
  1034b3:	81 fe 00 04 00 00    	cmp    esi,0x400
  1034b9:	0f 84 c4 00 00 00    	je     103583 <memory_dump+0xe3>
    {
        page_directorie_entry_t * pde = &pdir->entries[i];
        if (pde->Present)
  1034bf:	0f b6 04 b7          	movzx  eax,BYTE PTR [edi+esi*4]
  1034c3:	89 c2                	mov    edx,eax
  1034c5:	83 e2 01             	and    edx,0x1
  1034c8:	74 e6                	je     1034b0 <memory_dump+0x10>
        {
            printf("pdir[%d]={ PFN=%d PRESENT=%d WRITE=%d USER=%d }\n", i, pde->PageFrameNumber, pde->Present, pde->Write, pde->User);
  1034ca:	89 c1                	mov    ecx,eax
  1034cc:	d0 e8                	shr    al,1
  1034ce:	83 ec 08             	sub    esp,0x8
  1034d1:	c0 e9 02             	shr    cl,0x2
  1034d4:	83 e0 01             	and    eax,0x1
  1034d7:	0f b6 d2             	movzx  edx,dl
  1034da:	83 e1 01             	and    ecx,0x1
            page_table_t *ptable = (page_table_t *)(pde->PageFrameNumber * PAGE_SIZE);

            for(size_t i = 0; i < 1024; i++)
  1034dd:	31 db                	xor    ebx,ebx
            printf("pdir[%d]={ PFN=%d PRESENT=%d WRITE=%d USER=%d }\n", i, pde->PageFrameNumber, pde->Present, pde->Write, pde->User);
  1034df:	51                   	push   ecx
  1034e0:	50                   	push   eax
  1034e1:	52                   	push   edx
  1034e2:	0f b6 54 b7 01       	movzx  edx,BYTE PTR [edi+esi*4+0x1]
  1034e7:	0f b6 44 b7 02       	movzx  eax,BYTE PTR [edi+esi*4+0x2]
  1034ec:	c0 ea 04             	shr    dl,0x4
  1034ef:	c1 e0 04             	shl    eax,0x4
  1034f2:	0f b6 d2             	movzx  edx,dl
  1034f5:	09 c2                	or     edx,eax
  1034f7:	0f b6 44 b7 03       	movzx  eax,BYTE PTR [edi+esi*4+0x3]
  1034fc:	c1 e0 0c             	shl    eax,0xc
  1034ff:	09 d0                	or     eax,edx
  103501:	50                   	push   eax
  103502:	56                   	push   esi
  103503:	68 28 66 11 00       	push   0x116628
  103508:	e8 63 31 00 00       	call   106670 <printf>
            page_table_t *ptable = (page_table_t *)(pde->PageFrameNumber * PAGE_SIZE);
  10350d:	0f b6 44 b7 01       	movzx  eax,BYTE PTR [edi+esi*4+0x1]
  103512:	0f b6 6c b7 02       	movzx  ebp,BYTE PTR [edi+esi*4+0x2]
  103517:	83 c4 20             	add    esp,0x20
  10351a:	c0 e8 04             	shr    al,0x4
  10351d:	c1 e5 04             	shl    ebp,0x4
  103520:	0f b6 c0             	movzx  eax,al
  103523:	09 e8                	or     eax,ebp
  103525:	0f b6 6c b7 03       	movzx  ebp,BYTE PTR [edi+esi*4+0x3]
  10352a:	c1 e5 0c             	shl    ebp,0xc
  10352d:	09 c5                	or     ebp,eax
  10352f:	c1 e5 0c             	shl    ebp,0xc
  103532:	eb 13                	jmp    103547 <memory_dump+0xa7>
  103534:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
            for(size_t i = 0; i < 1024; i++)
  103538:	83 c3 01             	add    ebx,0x1
  10353b:	81 fb 00 04 00 00    	cmp    ebx,0x400
  103541:	0f 84 69 ff ff ff    	je     1034b0 <memory_dump+0x10>
            {
                page_t * p = &ptable->pages[i];
                if (p->Present)
  103547:	0f b6 44 9d 00       	movzx  eax,BYTE PTR [ebp+ebx*4+0x0]
  10354c:	a8 01                	test   al,0x1
  10354e:	74 e8                	je     103538 <memory_dump+0x98>
                {
                    printf("ptable[%d]={ PFN=%d PRESENT=%d WRITE=%d USER=%d }\n", i, p->PageFrameNumber, p->Present, p->Write, p->User);
  103550:	89 c2                	mov    edx,eax
  103552:	d0 e8                	shr    al,1
  103554:	83 ec 08             	sub    esp,0x8
  103557:	c0 ea 02             	shr    dl,0x2
  10355a:	83 e0 01             	and    eax,0x1
  10355d:	83 e2 01             	and    edx,0x1
  103560:	52                   	push   edx
  103561:	50                   	push   eax
  103562:	0f b6 44 9d 00       	movzx  eax,BYTE PTR [ebp+ebx*4+0x0]
  103567:	83 e0 01             	and    eax,0x1
  10356a:	50                   	push   eax
  10356b:	8b 44 9d 00          	mov    eax,DWORD PTR [ebp+ebx*4+0x0]
  10356f:	c1 e8 0c             	shr    eax,0xc
  103572:	50                   	push   eax
  103573:	53                   	push   ebx
  103574:	68 5c 66 11 00       	push   0x11665c
  103579:	e8 f2 30 00 00       	call   106670 <printf>
  10357e:	83 c4 20             	add    esp,0x20
  103581:	eb b5                	jmp    103538 <memory_dump+0x98>
                }
            }
        }
    }
}
  103583:	83 c4 0c             	add    esp,0xc
  103586:	5b                   	pop    ebx
  103587:	5e                   	pop    esi
  103588:	5f                   	pop    edi
  103589:	5d                   	pop    ebp
  10358a:	c3                   	ret    
  10358b:	66 90                	xchg   ax,ax
  10358d:	66 90                	xchg   ax,ax
  10358f:	90                   	nop

00103590 <sys_exit>:
#include "kernel/tasking.h"
#include "kernel/syscalls.h"
#include "kernel/logger.h"

int sys_exit(int code)
{
  103590:	83 ec 0c             	sub    esp,0xc
    thread_exit();
  103593:	e8 48 e6 ff ff       	call   101be0 <thread_exit>
    return code;
}
  103598:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  10359c:	83 c4 0c             	add    esp,0xc
  10359f:	c3                   	ret    

001035a0 <sys_print>:

int sys_print(const char *string)
{
  1035a0:	83 ec 18             	sub    esp,0x18
    printf(string);
  1035a3:	ff 74 24 1c          	push   DWORD PTR [esp+0x1c]
  1035a7:	e8 c4 30 00 00       	call   106670 <printf>
    return 0;
}
  1035ac:	31 c0                	xor    eax,eax
  1035ae:	83 c4 1c             	add    esp,0x1c
  1035b1:	c3                   	ret    
  1035b2:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
  1035b9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

001035c0 <sys_map>:

int sys_map(int memory, int size)
{
  1035c0:	83 ec 0c             	sub    esp,0xc
    STUB(memory, size);
  1035c3:	ff 74 24 14          	push   DWORD PTR [esp+0x14]
  1035c7:	ff 74 24 14          	push   DWORD PTR [esp+0x14]
  1035cb:	68 a4 1d 11 00       	push   0x111da4
  1035d0:	68 a8 1e 11 00       	push   0x111ea8
  1035d5:	e8 96 30 00 00       	call   106670 <printf>
    return 0;
}
  1035da:	31 c0                	xor    eax,eax
  1035dc:	83 c4 1c             	add    esp,0x1c
  1035df:	c3                   	ret    

001035e0 <sys_unmap>:

int sys_unmap(int memory, int size)
{
  1035e0:	83 ec 0c             	sub    esp,0xc
    STUB(memory, size);
  1035e3:	ff 74 24 14          	push   DWORD PTR [esp+0x14]
  1035e7:	ff 74 24 14          	push   DWORD PTR [esp+0x14]
  1035eb:	68 98 1d 11 00       	push   0x111d98
  1035f0:	68 a8 1e 11 00       	push   0x111ea8
  1035f5:	e8 76 30 00 00       	call   106670 <printf>
    return 0;
}
  1035fa:	31 c0                	xor    eax,eax
  1035fc:	83 c4 1c             	add    esp,0x1c
  1035ff:	c3                   	ret    

00103600 <syscall_dispatcher>:
        [2] = sys_map,
        [3] = sys_unmap,
};

void syscall_dispatcher(context_t *context)
{
  103600:	53                   	push   ebx
  103601:	83 ec 14             	sub    esp,0x14
  103604:	8b 5c 24 1c          	mov    ebx,DWORD PTR [esp+0x1c]
    // log("syscall %d (EBX=%d, ECX=%d, EDX=%d, ESI=%d, EDI=%d).", context->eax, context->ebx, context->ecx, context->edx, context->esi, context->edi);

    syscall_t syscall = (syscall_t)syscalls[context->eax];
  103608:	8b 43 2c             	mov    eax,DWORD PTR [ebx+0x2c]
    context->eax = syscall(context->ebx, context->ecx, context->edx, context->esi, context->edi);
  10360b:	ff 73 10             	push   DWORD PTR [ebx+0x10]
  10360e:	ff 73 14             	push   DWORD PTR [ebx+0x14]
  103611:	ff 73 24             	push   DWORD PTR [ebx+0x24]
  103614:	ff 73 28             	push   DWORD PTR [ebx+0x28]
  103617:	ff 73 20             	push   DWORD PTR [ebx+0x20]
  10361a:	ff 14 85 ac 1d 11 00 	call   DWORD PTR [eax*4+0x111dac]
  103621:	89 43 2c             	mov    DWORD PTR [ebx+0x2c],eax
  103624:	83 c4 28             	add    esp,0x28
  103627:	5b                   	pop    ebx
  103628:	c3                   	ret    
  103629:	66 90                	xchg   ax,ax
  10362b:	66 90                	xchg   ax,ax
  10362d:	66 90                	xchg   ax,ax
  10362f:	90                   	nop

00103630 <boot_screen>:
#include "sync/atomic.h"

multiboot_info_t mbootinfo;

void boot_screen(string msg)
{
  103630:	53                   	push   ebx
  103631:	83 ec 10             	sub    esp,0x10
  103634:	8b 5c 24 18          	mov    ebx,DWORD PTR [esp+0x18]
    vga_clear(vga_white, vga_black);
  103638:	6a 00                	push   0x0
  10363a:	6a 0f                	push   0xf
  10363c:	e8 7f 0a 00 00       	call   1040c0 <vga_clear>
    vga_text(36, 11, __kernel_name, vga_light_blue, vga_black);
  103641:	c7 04 24 00 00 00 00 	mov    DWORD PTR [esp],0x0
  103648:	6a 09                	push   0x9
  10364a:	ff 35 10 70 11 00    	push   DWORD PTR ds:0x117010
  103650:	6a 0b                	push   0xb
  103652:	6a 24                	push   0x24
  103654:	e8 b7 0b 00 00       	call   104210 <vga_text>
    vga_text(40 - strlen(msg) / 2, 13, msg, vga_white, vga_black);
  103659:	83 c4 14             	add    esp,0x14
  10365c:	53                   	push   ebx
  10365d:	e8 5e 2a 00 00       	call   1060c0 <strlen>
  103662:	d1 e8                	shr    eax,1
  103664:	c7 04 24 00 00 00 00 	mov    DWORD PTR [esp],0x0
  10366b:	6a 0f                	push   0xf
  10366d:	89 c2                	mov    edx,eax
  10366f:	b8 28 00 00 00       	mov    eax,0x28
  103674:	53                   	push   ebx
  103675:	29 d0                	sub    eax,edx
  103677:	6a 0d                	push   0xd
  103679:	50                   	push   eax
  10367a:	e8 91 0b 00 00       	call   104210 <vga_text>
}
  10367f:	83 c4 28             	add    esp,0x28
  103682:	5b                   	pop    ebx
  103683:	c3                   	ret    
  103684:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
  10368a:	8d bf 00 00 00 00    	lea    edi,[edi+0x0]

00103690 <get_kernel_end>:

extern int __end;
uint get_kernel_end(multiboot_info_t *minfo)
{
  103690:	53                   	push   ebx
  103691:	83 ec 14             	sub    esp,0x14
  103694:	8b 5c 24 1c          	mov    ebx,DWORD PTR [esp+0x1c]
    return max((uint)&__end, modules_get_end(minfo));
  103698:	53                   	push   ebx
  103699:	e8 42 dd ff ff       	call   1013e0 <modules_get_end>
  10369e:	83 c4 10             	add    esp,0x10
  1036a1:	3d 65 ee 23 00       	cmp    eax,0x23ee65
  1036a6:	73 10                	jae    1036b8 <get_kernel_end+0x28>
}
  1036a8:	83 c4 08             	add    esp,0x8
  1036ab:	b8 65 ee 23 00       	mov    eax,0x23ee65
  1036b0:	5b                   	pop    ebx
  1036b1:	c3                   	ret    
  1036b2:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
    return max((uint)&__end, modules_get_end(minfo));
  1036b8:	89 5c 24 10          	mov    DWORD PTR [esp+0x10],ebx
}
  1036bc:	83 c4 08             	add    esp,0x8
  1036bf:	5b                   	pop    ebx
    return max((uint)&__end, modules_get_end(minfo));
  1036c0:	e9 1b dd ff ff       	jmp    1013e0 <modules_get_end>
  1036c5:	66 90                	xchg   ax,ax
  1036c7:	66 90                	xchg   ax,ax
  1036c9:	66 90                	xchg   ax,ax
  1036cb:	66 90                	xchg   ax,ax
  1036cd:	66 90                	xchg   ax,ax
  1036cf:	90                   	nop

001036d0 <serial_setup>:
#include "kernel/serial.h"

#define PORT 0x3f8

void serial_setup()
{
  1036d0:	83 ec 14             	sub    esp,0x14
    // See: https://wiki.osdev.org/Serial_Ports
    outb(PORT + 1, 0x00);
  1036d3:	6a 00                	push   0x0
  1036d5:	68 f9 03 00 00       	push   0x3f9
  1036da:	e8 31 0d 00 00       	call   104410 <outb>
    outb(PORT + 3, 0x80);
  1036df:	58                   	pop    eax
  1036e0:	5a                   	pop    edx
  1036e1:	68 80 00 00 00       	push   0x80
  1036e6:	68 fb 03 00 00       	push   0x3fb
  1036eb:	e8 20 0d 00 00       	call   104410 <outb>
    outb(PORT + 0, 0x03);
  1036f0:	59                   	pop    ecx
  1036f1:	58                   	pop    eax
  1036f2:	6a 03                	push   0x3
  1036f4:	68 f8 03 00 00       	push   0x3f8
  1036f9:	e8 12 0d 00 00       	call   104410 <outb>
    outb(PORT + 1, 0x00);
  1036fe:	58                   	pop    eax
  1036ff:	5a                   	pop    edx
  103700:	6a 00                	push   0x0
  103702:	68 f9 03 00 00       	push   0x3f9
  103707:	e8 04 0d 00 00       	call   104410 <outb>
    outb(PORT + 3, 0x03);
  10370c:	59                   	pop    ecx
  10370d:	58                   	pop    eax
  10370e:	6a 03                	push   0x3
  103710:	68 fb 03 00 00       	push   0x3fb
  103715:	e8 f6 0c 00 00       	call   104410 <outb>
    outb(PORT + 2, 0xC7);
  10371a:	58                   	pop    eax
  10371b:	5a                   	pop    edx
  10371c:	68 c7 00 00 00       	push   0xc7
  103721:	68 fa 03 00 00       	push   0x3fa
  103726:	e8 e5 0c 00 00       	call   104410 <outb>
    outb(PORT + 4, 0x0B);
  10372b:	59                   	pop    ecx
  10372c:	58                   	pop    eax
  10372d:	6a 0b                	push   0xb
  10372f:	68 fc 03 00 00       	push   0x3fc
  103734:	e8 d7 0c 00 00       	call   104410 <outb>
}
  103739:	83 c4 1c             	add    esp,0x1c
  10373c:	c3                   	ret    
  10373d:	8d 76 00             	lea    esi,[esi+0x0]

00103740 <wait_read>:

void wait_read()
{
  103740:	83 ec 0c             	sub    esp,0xc
  103743:	90                   	nop
  103744:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
    while ((inb(PORT + 5) & 1) == 0)
  103748:	83 ec 0c             	sub    esp,0xc
  10374b:	68 fd 03 00 00       	push   0x3fd
  103750:	e8 ab 0c 00 00       	call   104400 <inb>
  103755:	83 c4 10             	add    esp,0x10
  103758:	a8 01                	test   al,0x1
  10375a:	74 ec                	je     103748 <wait_read+0x8>
        ;
}
  10375c:	83 c4 0c             	add    esp,0xc
  10375f:	c3                   	ret    

00103760 <wait_write>:

void wait_write()
{
  103760:	83 ec 0c             	sub    esp,0xc
  103763:	90                   	nop
  103764:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
    while ((inb(PORT + 5) & 0x20) == 0)
  103768:	83 ec 0c             	sub    esp,0xc
  10376b:	68 fd 03 00 00       	push   0x3fd
  103770:	e8 8b 0c 00 00       	call   104400 <inb>
  103775:	83 c4 10             	add    esp,0x10
  103778:	a8 20                	test   al,0x20
  10377a:	74 ec                	je     103768 <wait_write+0x8>
        ;
}
  10377c:	83 c4 0c             	add    esp,0xc
  10377f:	c3                   	ret    

00103780 <serial_getc>:

char serial_getc()
{
  103780:	83 ec 0c             	sub    esp,0xc
  103783:	90                   	nop
  103784:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
    while ((inb(PORT + 5) & 1) == 0)
  103788:	83 ec 0c             	sub    esp,0xc
  10378b:	68 fd 03 00 00       	push   0x3fd
  103790:	e8 6b 0c 00 00       	call   104400 <inb>
  103795:	83 c4 10             	add    esp,0x10
  103798:	a8 01                	test   al,0x1
  10379a:	74 ec                	je     103788 <serial_getc+0x8>
    wait_read();
    return inb(PORT);
  10379c:	83 ec 0c             	sub    esp,0xc
  10379f:	68 f8 03 00 00       	push   0x3f8
  1037a4:	e8 57 0c 00 00       	call   104400 <inb>
}
  1037a9:	83 c4 1c             	add    esp,0x1c
  1037ac:	c3                   	ret    
  1037ad:	8d 76 00             	lea    esi,[esi+0x0]

001037b0 <serial_putc>:

void serial_putc(char c)
{
  1037b0:	53                   	push   ebx
  1037b1:	83 ec 08             	sub    esp,0x8
  1037b4:	8b 5c 24 10          	mov    ebx,DWORD PTR [esp+0x10]
  1037b8:	90                   	nop
  1037b9:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
    while ((inb(PORT + 5) & 0x20) == 0)
  1037c0:	83 ec 0c             	sub    esp,0xc
  1037c3:	68 fd 03 00 00       	push   0x3fd
  1037c8:	e8 33 0c 00 00       	call   104400 <inb>
  1037cd:	83 c4 10             	add    esp,0x10
  1037d0:	a8 20                	test   al,0x20
  1037d2:	74 ec                	je     1037c0 <serial_putc+0x10>
    wait_write();
    outb(PORT, c);
  1037d4:	83 ec 08             	sub    esp,0x8
  1037d7:	0f b6 db             	movzx  ebx,bl
  1037da:	53                   	push   ebx
  1037db:	68 f8 03 00 00       	push   0x3f8
  1037e0:	e8 2b 0c 00 00       	call   104410 <outb>
}
  1037e5:	83 c4 18             	add    esp,0x18
  1037e8:	5b                   	pop    ebx
  1037e9:	c3                   	ret    
  1037ea:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]

001037f0 <serial_read>:

void serial_read(char *buffer, int size)
{
  1037f0:	83 ec 0c             	sub    esp,0xc
    STUB(buffer, size);
  1037f3:	ff 74 24 14          	push   DWORD PTR [esp+0x14]
  1037f7:	ff 74 24 14          	push   DWORD PTR [esp+0x14]
  1037fb:	68 c4 1d 11 00       	push   0x111dc4
  103800:	68 a8 1e 11 00       	push   0x111ea8
  103805:	e8 66 2e 00 00       	call   106670 <printf>
}
  10380a:	83 c4 1c             	add    esp,0x1c
  10380d:	c3                   	ret    
  10380e:	66 90                	xchg   ax,ax

00103810 <serial_print>:

void serial_print(const char *str)
{
  103810:	56                   	push   esi
  103811:	53                   	push   ebx
  103812:	83 ec 04             	sub    esp,0x4
  103815:	8b 74 24 10          	mov    esi,DWORD PTR [esp+0x10]
    for(int i = 0; str[i]; i++)
  103819:	0f b6 1e             	movzx  ebx,BYTE PTR [esi]
  10381c:	84 db                	test   bl,bl
  10381e:	74 38                	je     103858 <serial_print+0x48>
  103820:	83 c6 01             	add    esi,0x1
  103823:	90                   	nop
  103824:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
    while ((inb(PORT + 5) & 0x20) == 0)
  103828:	83 ec 0c             	sub    esp,0xc
  10382b:	68 fd 03 00 00       	push   0x3fd
  103830:	e8 cb 0b 00 00       	call   104400 <inb>
  103835:	83 c4 10             	add    esp,0x10
  103838:	a8 20                	test   al,0x20
  10383a:	74 ec                	je     103828 <serial_print+0x18>
    outb(PORT, c);
  10383c:	83 ec 08             	sub    esp,0x8
  10383f:	83 c6 01             	add    esi,0x1
  103842:	53                   	push   ebx
  103843:	68 f8 03 00 00       	push   0x3f8
  103848:	e8 c3 0b 00 00       	call   104410 <outb>
    for(int i = 0; str[i]; i++)
  10384d:	0f b6 5e ff          	movzx  ebx,BYTE PTR [esi-0x1]
  103851:	83 c4 10             	add    esp,0x10
  103854:	84 db                	test   bl,bl
  103856:	75 d0                	jne    103828 <serial_print+0x18>
    {
        serial_putc(str[i]);
    }   
  103858:	83 c4 04             	add    esp,0x4
  10385b:	5b                   	pop    ebx
  10385c:	5e                   	pop    esi
  10385d:	c3                   	ret    
  10385e:	66 90                	xchg   ax,ax

00103860 <atapio_common>:
#include "devices/atapio.h"
#include "kernel/logger.h"
#include "sync/atomic.h"

int atapio_common(u8 drive, u32 numblock, u8 count)
{
  103860:	57                   	push   edi
  103861:	56                   	push   esi
  103862:	53                   	push   ebx
  103863:	8b 5c 24 14          	mov    ebx,DWORD PTR [esp+0x14]
  103867:	8b 7c 24 18          	mov    edi,DWORD PTR [esp+0x18]
  10386b:	8b 74 24 10          	mov    esi,DWORD PTR [esp+0x10]
    outb(0x1F1, 0x00);                            /* NULL byte to port 0x1F1 */
  10386f:	83 ec 08             	sub    esp,0x8
  103872:	6a 00                	push   0x0
  103874:	68 f1 01 00 00       	push   0x1f1
    outb(0x1F3, (unsigned char)numblock);         /* Low 8 bits of the block address */
    outb(0x1F4, (unsigned char)(numblock >> 8));  /* Next 8 bits of the block address */
    outb(0x1F5, (unsigned char)(numblock >> 16)); /* Next 8 bits of the block address */

    /* Drive indicator, magic bits, and highest 4 bits of the block address */
    outb(0x1F6, 0xE0 | (drive << 4) | ((numblock >> 24) & 0x0F));
  103879:	c1 e6 04             	shl    esi,0x4
    outb(0x1F1, 0x00);                            /* NULL byte to port 0x1F1 */
  10387c:	e8 8f 0b 00 00       	call   104410 <outb>
    outb(0x1F2, count);                           /* Sector count */
  103881:	58                   	pop    eax
  103882:	89 f8                	mov    eax,edi
    outb(0x1F6, 0xE0 | (drive << 4) | ((numblock >> 24) & 0x0F));
  103884:	83 ce e0             	or     esi,0xffffffe0
    outb(0x1F2, count);                           /* Sector count */
  103887:	5a                   	pop    edx
  103888:	0f b6 f8             	movzx  edi,al
  10388b:	57                   	push   edi
  10388c:	68 f2 01 00 00       	push   0x1f2
  103891:	e8 7a 0b 00 00       	call   104410 <outb>
    outb(0x1F3, (unsigned char)numblock);         /* Low 8 bits of the block address */
  103896:	59                   	pop    ecx
  103897:	5f                   	pop    edi
  103898:	0f b6 c3             	movzx  eax,bl
  10389b:	50                   	push   eax
  10389c:	68 f3 01 00 00       	push   0x1f3
  1038a1:	e8 6a 0b 00 00       	call   104410 <outb>
    outb(0x1F4, (unsigned char)(numblock >> 8));  /* Next 8 bits of the block address */
  1038a6:	58                   	pop    eax
  1038a7:	5a                   	pop    edx
  1038a8:	0f b6 c7             	movzx  eax,bh
  1038ab:	50                   	push   eax
  1038ac:	68 f4 01 00 00       	push   0x1f4
  1038b1:	e8 5a 0b 00 00       	call   104410 <outb>
    outb(0x1F5, (unsigned char)(numblock >> 16)); /* Next 8 bits of the block address */
  1038b6:	89 d8                	mov    eax,ebx
    outb(0x1F6, 0xE0 | (drive << 4) | ((numblock >> 24) & 0x0F));
  1038b8:	c1 eb 18             	shr    ebx,0x18
    outb(0x1F5, (unsigned char)(numblock >> 16)); /* Next 8 bits of the block address */
  1038bb:	59                   	pop    ecx
  1038bc:	c1 e8 10             	shr    eax,0x10
    outb(0x1F6, 0xE0 | (drive << 4) | ((numblock >> 24) & 0x0F));
  1038bf:	83 e3 0f             	and    ebx,0xf
    outb(0x1F5, (unsigned char)(numblock >> 16)); /* Next 8 bits of the block address */
  1038c2:	5f                   	pop    edi
  1038c3:	0f b6 c0             	movzx  eax,al
    outb(0x1F6, 0xE0 | (drive << 4) | ((numblock >> 24) & 0x0F));
  1038c6:	09 f3                	or     ebx,esi
    outb(0x1F5, (unsigned char)(numblock >> 16)); /* Next 8 bits of the block address */
  1038c8:	50                   	push   eax
  1038c9:	68 f5 01 00 00       	push   0x1f5
    outb(0x1F6, 0xE0 | (drive << 4) | ((numblock >> 24) & 0x0F));
  1038ce:	0f b6 db             	movzx  ebx,bl
    outb(0x1F5, (unsigned char)(numblock >> 16)); /* Next 8 bits of the block address */
  1038d1:	e8 3a 0b 00 00       	call   104410 <outb>
    outb(0x1F6, 0xE0 | (drive << 4) | ((numblock >> 24) & 0x0F));
  1038d6:	58                   	pop    eax
  1038d7:	5a                   	pop    edx
  1038d8:	53                   	push   ebx
  1038d9:	68 f6 01 00 00       	push   0x1f6
  1038de:	e8 2d 0b 00 00       	call   104410 <outb>

    return 0;
  1038e3:	83 c4 10             	add    esp,0x10
}
  1038e6:	31 c0                	xor    eax,eax
  1038e8:	5b                   	pop    ebx
  1038e9:	5e                   	pop    esi
  1038ea:	5f                   	pop    edi
  1038eb:	c3                   	ret    
  1038ec:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]

001038f0 <atapio_wait>:

void atapio_wait()
{
  1038f0:	83 ec 0c             	sub    esp,0xc
  1038f3:	eb 07                	jmp    1038fc <atapio_wait+0xc>
  1038f5:	8d 76 00             	lea    esi,[esi+0x0]
    u8 status;

    do
    {
        status = inb(0x1F7);
    } while ((status & 0x80) && !(status & 0x08));
  1038f8:	a8 08                	test   al,0x8
  1038fa:	75 14                	jne    103910 <atapio_wait+0x20>
        status = inb(0x1F7);
  1038fc:	83 ec 0c             	sub    esp,0xc
  1038ff:	68 f7 01 00 00       	push   0x1f7
  103904:	e8 f7 0a 00 00       	call   104400 <inb>
    } while ((status & 0x80) && !(status & 0x08));
  103909:	83 c4 10             	add    esp,0x10
  10390c:	84 c0                	test   al,al
  10390e:	78 e8                	js     1038f8 <atapio_wait+0x8>
}
  103910:	83 c4 0c             	add    esp,0xc
  103913:	c3                   	ret    
  103914:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
  10391a:	8d bf 00 00 00 00    	lea    edi,[edi+0x0]

00103920 <atapio_read>:

int atapio_read(u8 drive, u32 numblock, u8 count, char *buf)
{
  103920:	57                   	push   edi
  103921:	56                   	push   esi
  103922:	53                   	push   ebx
  103923:	8b 7c 24 18          	mov    edi,DWORD PTR [esp+0x18]
  103927:	8b 5c 24 10          	mov    ebx,DWORD PTR [esp+0x10]
  10392b:	8b 74 24 14          	mov    esi,DWORD PTR [esp+0x14]
    atomic_begin();
  10392f:	e8 0c 45 00 00       	call   107e40 <atomic_begin>
    log("ATA::pio read drive:%d block:%d count:%d", drive, numblock, count);
  103934:	89 f8                	mov    eax,edi
  103936:	0f b6 f8             	movzx  edi,al
  103939:	0f b6 c3             	movzx  eax,bl
  10393c:	57                   	push   edi
  10393d:	56                   	push   esi
  10393e:	50                   	push   eax
  10393f:	68 b8 66 11 00       	push   0x1166b8
  103944:	e8 57 dd ff ff       	call   1016a0 <log>
    outb(0x1F1, 0x00);                            /* NULL byte to port 0x1F1 */
  103949:	58                   	pop    eax
  10394a:	5a                   	pop    edx
  10394b:	6a 00                	push   0x0
  10394d:	68 f1 01 00 00       	push   0x1f1
  103952:	e8 b9 0a 00 00       	call   104410 <outb>
    outb(0x1F2, count);                           /* Sector count */
  103957:	59                   	pop    ecx
  103958:	58                   	pop    eax
  103959:	57                   	push   edi
  10395a:	68 f2 01 00 00       	push   0x1f2
  10395f:	e8 ac 0a 00 00       	call   104410 <outb>
    outb(0x1F3, (unsigned char)numblock);         /* Low 8 bits of the block address */
  103964:	58                   	pop    eax
  103965:	89 f0                	mov    eax,esi
  103967:	5a                   	pop    edx
  103968:	0f b6 c0             	movzx  eax,al
  10396b:	50                   	push   eax
  10396c:	68 f3 01 00 00       	push   0x1f3
  103971:	e8 9a 0a 00 00       	call   104410 <outb>
    outb(0x1F4, (unsigned char)(numblock >> 8));  /* Next 8 bits of the block address */
  103976:	59                   	pop    ecx
  103977:	58                   	pop    eax
  103978:	89 f0                	mov    eax,esi
  10397a:	0f b6 c4             	movzx  eax,ah
  10397d:	50                   	push   eax
  10397e:	68 f4 01 00 00       	push   0x1f4
  103983:	e8 88 0a 00 00       	call   104410 <outb>
    outb(0x1F5, (unsigned char)(numblock >> 16)); /* Next 8 bits of the block address */
  103988:	58                   	pop    eax
  103989:	89 f0                	mov    eax,esi
    outb(0x1F6, 0xE0 | (drive << 4) | ((numblock >> 24) & 0x0F));
  10398b:	c1 ee 18             	shr    esi,0x18
    outb(0x1F5, (unsigned char)(numblock >> 16)); /* Next 8 bits of the block address */
  10398e:	5a                   	pop    edx
  10398f:	c1 e8 10             	shr    eax,0x10
    outb(0x1F6, 0xE0 | (drive << 4) | ((numblock >> 24) & 0x0F));
  103992:	83 e6 0f             	and    esi,0xf
    outb(0x1F5, (unsigned char)(numblock >> 16)); /* Next 8 bits of the block address */
  103995:	0f b6 c0             	movzx  eax,al
  103998:	50                   	push   eax
  103999:	68 f5 01 00 00       	push   0x1f5
  10399e:	e8 6d 0a 00 00       	call   104410 <outb>
    outb(0x1F6, 0xE0 | (drive << 4) | ((numblock >> 24) & 0x0F));
  1039a3:	59                   	pop    ecx
  1039a4:	58                   	pop    eax
  1039a5:	89 d8                	mov    eax,ebx
  1039a7:	c1 e0 04             	shl    eax,0x4
  1039aa:	83 c8 e0             	or     eax,0xffffffe0
  1039ad:	09 f0                	or     eax,esi
  1039af:	0f b6 c0             	movzx  eax,al
  1039b2:	50                   	push   eax
  1039b3:	68 f6 01 00 00       	push   0x1f6
  1039b8:	e8 53 0a 00 00       	call   104410 <outb>
    u16 tmpword;
    int idx;

    atapio_common(drive, numblock, count);
    outb(0x1F7, 0x20);
  1039bd:	58                   	pop    eax
  1039be:	5a                   	pop    edx
  1039bf:	6a 20                	push   0x20
  1039c1:	68 f7 01 00 00       	push   0x1f7
  1039c6:	e8 45 0a 00 00       	call   104410 <outb>
  1039cb:	83 c4 10             	add    esp,0x10
  1039ce:	eb 04                	jmp    1039d4 <atapio_read+0xb4>
    } while ((status & 0x80) && !(status & 0x08));
  1039d0:	a8 08                	test   al,0x8
  1039d2:	75 14                	jne    1039e8 <atapio_read+0xc8>
        status = inb(0x1F7);
  1039d4:	83 ec 0c             	sub    esp,0xc
  1039d7:	68 f7 01 00 00       	push   0x1f7
  1039dc:	e8 1f 0a 00 00       	call   104400 <inb>
    } while ((status & 0x80) && !(status & 0x08));
  1039e1:	83 c4 10             	add    esp,0x10
  1039e4:	84 c0                	test   al,al
  1039e6:	78 e8                	js     1039d0 <atapio_read+0xb0>

    /* Wait for the drive to signal that it's ready: */
    atapio_wait();

    for (idx = 0; idx < 256 * count; idx++)
  1039e8:	85 ff                	test   edi,edi
  1039ea:	74 35                	je     103a21 <atapio_read+0x101>
  1039ec:	8b 5c 24 1c          	mov    ebx,DWORD PTR [esp+0x1c]
  1039f0:	89 fe                	mov    esi,edi
  1039f2:	c1 e6 09             	shl    esi,0x9
  1039f5:	01 de                	add    esi,ebx
  1039f7:	89 f6                	mov    esi,esi
  1039f9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
    {
        tmpword = inw(0x1F0);
  103a00:	83 ec 0c             	sub    esp,0xc
  103a03:	83 c3 02             	add    ebx,0x2
  103a06:	68 f0 01 00 00       	push   0x1f0
  103a0b:	e8 10 0a 00 00       	call   104420 <inw>
        buf[idx * 2] = (unsigned char)tmpword;
  103a10:	88 43 fe             	mov    BYTE PTR [ebx-0x2],al
        buf[idx * 2 + 1] = (unsigned char)(tmpword >> 8);
  103a13:	66 c1 e8 08          	shr    ax,0x8
    for (idx = 0; idx < 256 * count; idx++)
  103a17:	83 c4 10             	add    esp,0x10
        buf[idx * 2 + 1] = (unsigned char)(tmpword >> 8);
  103a1a:	88 43 ff             	mov    BYTE PTR [ebx-0x1],al
    for (idx = 0; idx < 256 * count; idx++)
  103a1d:	39 de                	cmp    esi,ebx
  103a1f:	75 df                	jne    103a00 <atapio_read+0xe0>
    }

    log("ATA::pio read done!");
  103a21:	83 ec 0c             	sub    esp,0xc
  103a24:	68 9e 22 11 00       	push   0x11229e
  103a29:	e8 72 dc ff ff       	call   1016a0 <log>
    atomic_end();
  103a2e:	e8 2d 44 00 00       	call   107e60 <atomic_end>

    return count;
  103a33:	83 c4 10             	add    esp,0x10
}
  103a36:	89 f8                	mov    eax,edi
  103a38:	5b                   	pop    ebx
  103a39:	5e                   	pop    esi
  103a3a:	5f                   	pop    edi
  103a3b:	c3                   	ret    
  103a3c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]

00103a40 <atapio_write>:

int atapio_write(u8 drive, u32 numblock, u8 count, char *buf)
{
  103a40:	57                   	push   edi
  103a41:	56                   	push   esi
  103a42:	53                   	push   ebx
  103a43:	8b 74 24 18          	mov    esi,DWORD PTR [esp+0x18]
  103a47:	8b 5c 24 10          	mov    ebx,DWORD PTR [esp+0x10]
  103a4b:	8b 7c 24 14          	mov    edi,DWORD PTR [esp+0x14]
    atomic_begin();
  103a4f:	e8 ec 43 00 00       	call   107e40 <atomic_begin>
    log("ATA::pio write drive:%d block:%d count:%d", drive, numblock, count);
  103a54:	89 f0                	mov    eax,esi
  103a56:	0f b6 f0             	movzx  esi,al
  103a59:	0f b6 c3             	movzx  eax,bl
  103a5c:	56                   	push   esi
  103a5d:	57                   	push   edi
  103a5e:	50                   	push   eax
  103a5f:	68 e4 66 11 00       	push   0x1166e4
  103a64:	e8 37 dc ff ff       	call   1016a0 <log>
    outb(0x1F1, 0x00);                            /* NULL byte to port 0x1F1 */
  103a69:	58                   	pop    eax
  103a6a:	5a                   	pop    edx
  103a6b:	6a 00                	push   0x0
  103a6d:	68 f1 01 00 00       	push   0x1f1
  103a72:	e8 99 09 00 00       	call   104410 <outb>
    outb(0x1F2, count);                           /* Sector count */
  103a77:	59                   	pop    ecx
  103a78:	58                   	pop    eax
  103a79:	56                   	push   esi
  103a7a:	68 f2 01 00 00       	push   0x1f2
  103a7f:	e8 8c 09 00 00       	call   104410 <outb>
    outb(0x1F3, (unsigned char)numblock);         /* Low 8 bits of the block address */
  103a84:	58                   	pop    eax
  103a85:	89 f8                	mov    eax,edi
  103a87:	5a                   	pop    edx
  103a88:	0f b6 c0             	movzx  eax,al
  103a8b:	50                   	push   eax
  103a8c:	68 f3 01 00 00       	push   0x1f3
  103a91:	e8 7a 09 00 00       	call   104410 <outb>
    outb(0x1F4, (unsigned char)(numblock >> 8));  /* Next 8 bits of the block address */
  103a96:	59                   	pop    ecx
  103a97:	58                   	pop    eax
  103a98:	89 f8                	mov    eax,edi
  103a9a:	0f b6 c4             	movzx  eax,ah
  103a9d:	50                   	push   eax
  103a9e:	68 f4 01 00 00       	push   0x1f4
  103aa3:	e8 68 09 00 00       	call   104410 <outb>
    outb(0x1F5, (unsigned char)(numblock >> 16)); /* Next 8 bits of the block address */
  103aa8:	58                   	pop    eax
  103aa9:	89 f8                	mov    eax,edi
    outb(0x1F6, 0xE0 | (drive << 4) | ((numblock >> 24) & 0x0F));
  103aab:	c1 ef 18             	shr    edi,0x18
    outb(0x1F5, (unsigned char)(numblock >> 16)); /* Next 8 bits of the block address */
  103aae:	5a                   	pop    edx
  103aaf:	c1 e8 10             	shr    eax,0x10
    outb(0x1F6, 0xE0 | (drive << 4) | ((numblock >> 24) & 0x0F));
  103ab2:	83 e7 0f             	and    edi,0xf
    outb(0x1F5, (unsigned char)(numblock >> 16)); /* Next 8 bits of the block address */
  103ab5:	0f b6 c0             	movzx  eax,al
  103ab8:	50                   	push   eax
  103ab9:	68 f5 01 00 00       	push   0x1f5
  103abe:	e8 4d 09 00 00       	call   104410 <outb>
    outb(0x1F6, 0xE0 | (drive << 4) | ((numblock >> 24) & 0x0F));
  103ac3:	59                   	pop    ecx
  103ac4:	58                   	pop    eax
  103ac5:	89 d8                	mov    eax,ebx
  103ac7:	c1 e0 04             	shl    eax,0x4
  103aca:	83 c8 e0             	or     eax,0xffffffe0
  103acd:	09 f8                	or     eax,edi
  103acf:	0f b6 c0             	movzx  eax,al
  103ad2:	50                   	push   eax
  103ad3:	68 f6 01 00 00       	push   0x1f6
  103ad8:	e8 33 09 00 00       	call   104410 <outb>

    u16 tmpword;

    atapio_common(drive, numblock, count);
    outb(0x1F7, 0x30);
  103add:	58                   	pop    eax
  103ade:	5a                   	pop    edx
  103adf:	6a 30                	push   0x30
  103ae1:	68 f7 01 00 00       	push   0x1f7
  103ae6:	e8 25 09 00 00       	call   104410 <outb>
  103aeb:	83 c4 10             	add    esp,0x10
  103aee:	eb 04                	jmp    103af4 <atapio_write+0xb4>
    } while ((status & 0x80) && !(status & 0x08));
  103af0:	a8 08                	test   al,0x8
  103af2:	75 14                	jne    103b08 <atapio_write+0xc8>
        status = inb(0x1F7);
  103af4:	83 ec 0c             	sub    esp,0xc
  103af7:	68 f7 01 00 00       	push   0x1f7
  103afc:	e8 ff 08 00 00       	call   104400 <inb>
    } while ((status & 0x80) && !(status & 0x08));
  103b01:	83 c4 10             	add    esp,0x10
  103b04:	84 c0                	test   al,al
  103b06:	78 e8                	js     103af0 <atapio_write+0xb0>

    /* Wait for the drive to signal that it's ready: */
    atapio_wait();

    for (int i = 0; i < 256 * count; i++)
  103b08:	85 f6                	test   esi,esi
  103b0a:	74 3f                	je     103b4b <atapio_write+0x10b>
  103b0c:	8b 7c 24 1c          	mov    edi,DWORD PTR [esp+0x1c]
  103b10:	89 f3                	mov    ebx,esi
  103b12:	c1 e3 09             	shl    ebx,0x9
  103b15:	01 fb                	add    ebx,edi
  103b17:	89 f6                	mov    esi,esi
  103b19:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
    {
        tmpword = (buf[i * 2 + 1] << 8) | buf[i * 2];
  103b20:	0f be 47 01          	movsx  eax,BYTE PTR [edi+0x1]
        outw(0x1F0, tmpword);
  103b24:	83 ec 08             	sub    esp,0x8
  103b27:	83 c7 02             	add    edi,0x2
        tmpword = (buf[i * 2 + 1] << 8) | buf[i * 2];
  103b2a:	c1 e0 08             	shl    eax,0x8
  103b2d:	89 c2                	mov    edx,eax
  103b2f:	66 0f be 47 fe       	movsx  ax,BYTE PTR [edi-0x2]
  103b34:	09 d0                	or     eax,edx
        outw(0x1F0, tmpword);
  103b36:	0f b7 c0             	movzx  eax,ax
  103b39:	50                   	push   eax
  103b3a:	68 f0 01 00 00       	push   0x1f0
  103b3f:	e8 ec 08 00 00       	call   104430 <outw>
    for (int i = 0; i < 256 * count; i++)
  103b44:	83 c4 10             	add    esp,0x10
  103b47:	39 fb                	cmp    ebx,edi
  103b49:	75 d5                	jne    103b20 <atapio_write+0xe0>
    }

    log("ATA::pio write done!");
  103b4b:	83 ec 0c             	sub    esp,0xc
  103b4e:	68 b2 22 11 00       	push   0x1122b2
  103b53:	e8 48 db ff ff       	call   1016a0 <log>
    atomic_end();
  103b58:	e8 03 43 00 00       	call   107e60 <atomic_end>

    return count;
  103b5d:	83 c4 10             	add    esp,0x10
  103b60:	89 f0                	mov    eax,esi
  103b62:	5b                   	pop    ebx
  103b63:	5e                   	pop    esi
  103b64:	5f                   	pop    edi
  103b65:	c3                   	ret    
  103b66:	66 90                	xchg   ax,ax
  103b68:	66 90                	xchg   ax,ax
  103b6a:	66 90                	xchg   ax,ax
  103b6c:	66 90                	xchg   ax,ax
  103b6e:	66 90                	xchg   ax,ax

00103b70 <timer_handler>:
uint ticks = 0;

esp_t timer_handler(esp_t esp, context_t *context)
{
    UNUSED(context);
    ticks++;
  103b70:	83 05 1c ae 23 00 01 	add    DWORD PTR ds:0x23ae1c,0x1
    return esp;
}
  103b77:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  103b7b:	c3                   	ret    
  103b7c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]

00103b80 <timer_setup>:

/* --- Public Functions ----------------------------------------------------- */

void timer_setup()
{
  103b80:	83 ec 14             	sub    esp,0x14
}

void timer_set_frequency(int hz)
{
    u32 divisor = 1193180 / hz;
    outb(0x43, 0x36);
  103b83:	6a 36                	push   0x36
  103b85:	6a 43                	push   0x43
  103b87:	e8 84 08 00 00       	call   104410 <outb>
    outb(0x40, divisor & 0xFF);
  103b8c:	58                   	pop    eax
  103b8d:	5a                   	pop    edx
  103b8e:	68 a9 00 00 00       	push   0xa9
  103b93:	6a 40                	push   0x40
  103b95:	e8 76 08 00 00       	call   104410 <outb>
    outb(0x40, (divisor >> 8) & 0xFF);
  103b9a:	59                   	pop    ecx
  103b9b:	58                   	pop    eax
  103b9c:	6a 04                	push   0x4
  103b9e:	6a 40                	push   0x40
  103ba0:	e8 6b 08 00 00       	call   104410 <outb>

    log("Timer frequency is %dhz.", hz);
  103ba5:	58                   	pop    eax
  103ba6:	5a                   	pop    edx
  103ba7:	68 e8 03 00 00       	push   0x3e8
  103bac:	68 c7 22 11 00       	push   0x1122c7
  103bb1:	e8 ea da ff ff       	call   1016a0 <log>
    irq_register(0, (irq_handler_t)&timer_handler);
  103bb6:	59                   	pop    ecx
  103bb7:	58                   	pop    eax
  103bb8:	68 70 3b 10 00       	push   0x103b70
  103bbd:	6a 00                	push   0x0
  103bbf:	e8 bc 07 00 00       	call   104380 <irq_register>
}
  103bc4:	83 c4 1c             	add    esp,0x1c
  103bc7:	c3                   	ret    
  103bc8:	90                   	nop
  103bc9:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]

00103bd0 <timer_set_frequency>:
{
  103bd0:	56                   	push   esi
  103bd1:	53                   	push   ebx
    u32 divisor = 1193180 / hz;
  103bd2:	b8 dc 34 12 00       	mov    eax,0x1234dc
  103bd7:	99                   	cdq    
{
  103bd8:	83 ec 0c             	sub    esp,0xc
  103bdb:	8b 74 24 18          	mov    esi,DWORD PTR [esp+0x18]
    outb(0x43, 0x36);
  103bdf:	6a 36                	push   0x36
  103be1:	6a 43                	push   0x43
    u32 divisor = 1193180 / hz;
  103be3:	f7 fe                	idiv   esi
  103be5:	89 c3                	mov    ebx,eax
    outb(0x43, 0x36);
  103be7:	e8 24 08 00 00       	call   104410 <outb>
    outb(0x40, divisor & 0xFF);
  103bec:	58                   	pop    eax
  103bed:	5a                   	pop    edx
  103bee:	0f b6 c3             	movzx  eax,bl
    outb(0x40, (divisor >> 8) & 0xFF);
  103bf1:	0f b6 df             	movzx  ebx,bh
    outb(0x40, divisor & 0xFF);
  103bf4:	50                   	push   eax
  103bf5:	6a 40                	push   0x40
  103bf7:	e8 14 08 00 00       	call   104410 <outb>
    outb(0x40, (divisor >> 8) & 0xFF);
  103bfc:	59                   	pop    ecx
  103bfd:	58                   	pop    eax
  103bfe:	53                   	push   ebx
  103bff:	6a 40                	push   0x40
  103c01:	e8 0a 08 00 00       	call   104410 <outb>
    log("Timer frequency is %dhz.", hz);
  103c06:	58                   	pop    eax
  103c07:	5a                   	pop    edx
  103c08:	56                   	push   esi
  103c09:	68 c7 22 11 00       	push   0x1122c7
  103c0e:	e8 8d da ff ff       	call   1016a0 <log>
}
  103c13:	83 c4 14             	add    esp,0x14
  103c16:	5b                   	pop    ebx
  103c17:	5e                   	pop    esi
  103c18:	c3                   	ret    
  103c19:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]

00103c20 <timer_get_ticks>:

u32 timer_get_ticks()
{
    return ticks;
}
  103c20:	a1 1c ae 23 00       	mov    eax,ds:0x23ae1c
  103c25:	c3                   	ret    
  103c26:	66 90                	xchg   ax,ax
  103c28:	66 90                	xchg   ax,ax
  103c2a:	66 90                	xchg   ax,ax
  103c2c:	66 90                	xchg   ax,ax
  103c2e:	66 90                	xchg   ax,ax

00103c30 <bga_write_register>:
#include "cpu/cpu.h"
#include "devices/bga.h"
#include "kernel/logger.h"

void bga_write_register(u16 IndexValue, u16 DataValue)
{
  103c30:	53                   	push   ebx
  103c31:	83 ec 10             	sub    esp,0x10
    outw(VBE_DISPI_IOPORT_INDEX, IndexValue);
  103c34:	0f b7 44 24 18       	movzx  eax,WORD PTR [esp+0x18]
{
  103c39:	8b 5c 24 1c          	mov    ebx,DWORD PTR [esp+0x1c]
    outw(VBE_DISPI_IOPORT_DATA, DataValue);
  103c3d:	0f b7 db             	movzx  ebx,bx
    outw(VBE_DISPI_IOPORT_INDEX, IndexValue);
  103c40:	50                   	push   eax
  103c41:	68 ce 01 00 00       	push   0x1ce
  103c46:	e8 e5 07 00 00       	call   104430 <outw>
    outw(VBE_DISPI_IOPORT_DATA, DataValue);
  103c4b:	89 5c 24 24          	mov    DWORD PTR [esp+0x24],ebx
  103c4f:	c7 44 24 20 cf 01 00 	mov    DWORD PTR [esp+0x20],0x1cf
  103c56:	00 
}
  103c57:	83 c4 18             	add    esp,0x18
  103c5a:	5b                   	pop    ebx
    outw(VBE_DISPI_IOPORT_DATA, DataValue);
  103c5b:	e9 d0 07 00 00       	jmp    104430 <outw>

00103c60 <bga_read_register>:

u16 bga_read_register(u16 IndexValue)
{
  103c60:	83 ec 14             	sub    esp,0x14
    outw(VBE_DISPI_IOPORT_INDEX, IndexValue);
  103c63:	0f b7 44 24 18       	movzx  eax,WORD PTR [esp+0x18]
  103c68:	50                   	push   eax
  103c69:	68 ce 01 00 00       	push   0x1ce
  103c6e:	e8 bd 07 00 00       	call   104430 <outw>
    return inw(VBE_DISPI_IOPORT_DATA);
  103c73:	c7 44 24 20 cf 01 00 	mov    DWORD PTR [esp+0x20],0x1cf
  103c7a:	00 
}
  103c7b:	83 c4 1c             	add    esp,0x1c
    return inw(VBE_DISPI_IOPORT_DATA);
  103c7e:	e9 9d 07 00 00       	jmp    104420 <inw>
  103c83:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
  103c89:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00103c90 <bga_set_bank>:

void bga_set_bank(u16 bank_number)
{
  103c90:	53                   	push   ebx
  103c91:	83 ec 10             	sub    esp,0x10
  103c94:	8b 5c 24 18          	mov    ebx,DWORD PTR [esp+0x18]
    outw(VBE_DISPI_IOPORT_INDEX, IndexValue);
  103c98:	6a 05                	push   0x5
  103c9a:	68 ce 01 00 00       	push   0x1ce
  103c9f:	e8 8c 07 00 00       	call   104430 <outw>
    outw(VBE_DISPI_IOPORT_DATA, DataValue);
  103ca4:	58                   	pop    eax
  103ca5:	5a                   	pop    edx
    bga_write_register(VBE_DISPI_INDEX_BANK, bank_number);
  103ca6:	0f b7 db             	movzx  ebx,bx
    outw(VBE_DISPI_IOPORT_DATA, DataValue);
  103ca9:	53                   	push   ebx
  103caa:	68 cf 01 00 00       	push   0x1cf
  103caf:	e8 7c 07 00 00       	call   104430 <outw>
}
  103cb4:	83 c4 18             	add    esp,0x18
  103cb7:	5b                   	pop    ebx
  103cb8:	c3                   	ret    
  103cb9:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]

00103cc0 <bga_is_available>:

bool bga_is_available(void)
{
  103cc0:	56                   	push   esi
  103cc1:	53                   	push   ebx
    bool found = false;
  103cc2:	31 f6                	xor    esi,esi
    for (u32 i = 0; i < 6; i++)
  103cc4:	31 db                	xor    ebx,ebx
{
  103cc6:	83 ec 04             	sub    esp,0x4
  103cc9:	eb 0d                	jmp    103cd8 <bga_is_available+0x18>
  103ccb:	90                   	nop
  103ccc:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
    for (u32 i = 0; i < 6; i++)
  103cd0:	83 c3 01             	add    ebx,0x1
  103cd3:	83 fb 06             	cmp    ebx,0x6
  103cd6:	74 49                	je     103d21 <bga_is_available+0x61>
    outw(VBE_DISPI_IOPORT_INDEX, IndexValue);
  103cd8:	83 ec 08             	sub    esp,0x8
  103cdb:	6a 00                	push   0x0
  103cdd:	68 ce 01 00 00       	push   0x1ce
  103ce2:	e8 49 07 00 00       	call   104430 <outw>
    return inw(VBE_DISPI_IOPORT_DATA);
  103ce7:	c7 04 24 cf 01 00 00 	mov    DWORD PTR [esp],0x1cf
  103cee:	e8 2d 07 00 00       	call   104420 <inw>
    {
        if (bga_read_register(VBE_DISPI_INDEX_ID) == 0xB0C0 + i)
  103cf3:	8d 93 c0 b0 00 00    	lea    edx,[ebx+0xb0c0]
  103cf9:	0f b7 c0             	movzx  eax,ax
  103cfc:	83 c4 10             	add    esp,0x10
  103cff:	39 d0                	cmp    eax,edx
  103d01:	75 cd                	jne    103cd0 <bga_is_available+0x10>
        {
            log("BGA video device detected (version %x).", i);
  103d03:	83 ec 08             	sub    esp,0x8
            found = true;
  103d06:	be 01 00 00 00       	mov    esi,0x1
            log("BGA video device detected (version %x).", i);
  103d0b:	53                   	push   ebx
  103d0c:	68 10 67 11 00       	push   0x116710
    for (u32 i = 0; i < 6; i++)
  103d11:	83 c3 01             	add    ebx,0x1
            log("BGA video device detected (version %x).", i);
  103d14:	e8 87 d9 ff ff       	call   1016a0 <log>
  103d19:	83 c4 10             	add    esp,0x10
    for (u32 i = 0; i < 6; i++)
  103d1c:	83 fb 06             	cmp    ebx,0x6
  103d1f:	75 b7                	jne    103cd8 <bga_is_available+0x18>
        }
    }
    return found;
}
  103d21:	83 c4 04             	add    esp,0x4
  103d24:	89 f0                	mov    eax,esi
  103d26:	5b                   	pop    ebx
  103d27:	5e                   	pop    esi
  103d28:	c3                   	ret    
  103d29:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]

00103d30 <bga_get_framebuffer>:

u32 * bga_get_framebuffer()
{
  103d30:	53                   	push   ebx
    bga_set_bank(0);
    u32 * lfb = 0;
    u32 * text_vid_mem = (u32 *)0xA0000;
    text_vid_mem[0] = 0xA5ADFACE;

    for (u32 fb_offset = 0xE0000000; fb_offset < 0xFF000000; fb_offset += 0x01000000)
  103d31:	bb 00 00 00 e0       	mov    ebx,0xe0000000
{
  103d36:	83 ec 10             	sub    esp,0x10
    outw(VBE_DISPI_IOPORT_INDEX, IndexValue);
  103d39:	6a 05                	push   0x5
  103d3b:	68 ce 01 00 00       	push   0x1ce
  103d40:	e8 eb 06 00 00       	call   104430 <outw>
    outw(VBE_DISPI_IOPORT_DATA, DataValue);
  103d45:	58                   	pop    eax
  103d46:	5a                   	pop    edx
  103d47:	6a 00                	push   0x0
  103d49:	68 cf 01 00 00       	push   0x1cf
  103d4e:	e8 dd 06 00 00       	call   104430 <outw>
    text_vid_mem[0] = 0xA5ADFACE;
  103d53:	c7 05 00 00 0a 00 ce 	mov    DWORD PTR ds:0xa0000,0xa5adface
  103d5a:	fa ad a5 
  103d5d:	83 c4 10             	add    esp,0x10
    u32 * lfb = 0;
  103d60:	31 c0                	xor    eax,eax
  103d62:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
  103d68:	8d 8b 00 00 ff 00    	lea    ecx,[ebx+0xff0000]
    {

        /* Go find it */
        for (u32 x = fb_offset; x < fb_offset + 0xFF0000; x += 0x1000)
  103d6e:	39 d9                	cmp    ecx,ebx
  103d70:	76 19                	jbe    103d8b <bga_get_framebuffer+0x5b>
  103d72:	89 da                	mov    edx,ebx
  103d74:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
        {
            if (((u32 *)x)[0] == 0xA5ADFACE)
  103d78:	81 3a ce fa ad a5    	cmp    DWORD PTR [edx],0xa5adface
  103d7e:	0f 44 c2             	cmove  eax,edx
        for (u32 x = fb_offset; x < fb_offset + 0xFF0000; x += 0x1000)
  103d81:	81 c2 00 10 00 00    	add    edx,0x1000
  103d87:	39 d1                	cmp    ecx,edx
  103d89:	77 ed                	ja     103d78 <bga_get_framebuffer+0x48>
    for (u32 fb_offset = 0xE0000000; fb_offset < 0xFF000000; fb_offset += 0x01000000)
  103d8b:	81 c3 00 00 00 01    	add    ebx,0x1000000
  103d91:	81 fb 00 00 00 ff    	cmp    ebx,0xff000000
  103d97:	75 cf                	jne    103d68 <bga_get_framebuffer+0x38>
            }
        }
    }

    return lfb;
}
  103d99:	83 c4 08             	add    esp,0x8
  103d9c:	5b                   	pop    ebx
  103d9d:	c3                   	ret    
  103d9e:	66 90                	xchg   ax,ax

00103da0 <bga_setup>:
u32 bga_width = 0;
u32 bga_height = 0;
u32 * bga_framebuffer = NULL;

void bga_setup()
{
  103da0:	56                   	push   esi
  103da1:	53                   	push   ebx
    bool found = false;
  103da2:	31 f6                	xor    esi,esi
    for (u32 i = 0; i < 6; i++)
  103da4:	31 db                	xor    ebx,ebx
{
  103da6:	83 ec 04             	sub    esp,0x4
  103da9:	eb 08                	jmp    103db3 <bga_setup+0x13>
    for (u32 i = 0; i < 6; i++)
  103dab:	83 c3 01             	add    ebx,0x1
  103dae:	83 fb 06             	cmp    ebx,0x6
  103db1:	74 49                	je     103dfc <bga_setup+0x5c>
    outw(VBE_DISPI_IOPORT_INDEX, IndexValue);
  103db3:	83 ec 08             	sub    esp,0x8
  103db6:	6a 00                	push   0x0
  103db8:	68 ce 01 00 00       	push   0x1ce
  103dbd:	e8 6e 06 00 00       	call   104430 <outw>
    return inw(VBE_DISPI_IOPORT_DATA);
  103dc2:	c7 04 24 cf 01 00 00 	mov    DWORD PTR [esp],0x1cf
  103dc9:	e8 52 06 00 00       	call   104420 <inw>
        if (bga_read_register(VBE_DISPI_INDEX_ID) == 0xB0C0 + i)
  103dce:	8d 93 c0 b0 00 00    	lea    edx,[ebx+0xb0c0]
  103dd4:	0f b7 c0             	movzx  eax,ax
  103dd7:	83 c4 10             	add    esp,0x10
  103dda:	39 d0                	cmp    eax,edx
  103ddc:	75 cd                	jne    103dab <bga_setup+0xb>
            log("BGA video device detected (version %x).", i);
  103dde:	83 ec 08             	sub    esp,0x8
            found = true;
  103de1:	be 01 00 00 00       	mov    esi,0x1
            log("BGA video device detected (version %x).", i);
  103de6:	53                   	push   ebx
  103de7:	68 10 67 11 00       	push   0x116710
    for (u32 i = 0; i < 6; i++)
  103dec:	83 c3 01             	add    ebx,0x1
            log("BGA video device detected (version %x).", i);
  103def:	e8 ac d8 ff ff       	call   1016a0 <log>
  103df4:	83 c4 10             	add    esp,0x10
    for (u32 i = 0; i < 6; i++)
  103df7:	83 fb 06             	cmp    ebx,0x6
  103dfa:	75 b7                	jne    103db3 <bga_setup+0x13>
    if (bga_is_available())
  103dfc:	89 f0                	mov    eax,esi
  103dfe:	84 c0                	test   al,al
  103e00:	75 06                	jne    103e08 <bga_setup+0x68>
    {
        bga_mode(1280, 720);
        bga_framebuffer = bga_get_framebuffer();
    }
}
  103e02:	83 c4 04             	add    esp,0x4
  103e05:	5b                   	pop    ebx
  103e06:	5e                   	pop    esi
  103e07:	c3                   	ret    
    outw(VBE_DISPI_IOPORT_INDEX, IndexValue);
  103e08:	83 ec 08             	sub    esp,0x8

void bga_mode(u32 width, u32 height)
{
    bga_width = width;
  103e0b:	c7 05 28 ae 23 00 00 	mov    DWORD PTR ds:0x23ae28,0x500
  103e12:	05 00 00 
    bga_height = height;
  103e15:	c7 05 24 ae 23 00 d0 	mov    DWORD PTR ds:0x23ae24,0x2d0
  103e1c:	02 00 00 
    outw(VBE_DISPI_IOPORT_INDEX, IndexValue);
  103e1f:	6a 04                	push   0x4
  103e21:	68 ce 01 00 00       	push   0x1ce
  103e26:	e8 05 06 00 00       	call   104430 <outw>
    outw(VBE_DISPI_IOPORT_DATA, DataValue);
  103e2b:	58                   	pop    eax
  103e2c:	5a                   	pop    edx
  103e2d:	6a 00                	push   0x0
  103e2f:	68 cf 01 00 00       	push   0x1cf
  103e34:	e8 f7 05 00 00       	call   104430 <outw>
    outw(VBE_DISPI_IOPORT_INDEX, IndexValue);
  103e39:	59                   	pop    ecx
  103e3a:	5b                   	pop    ebx
  103e3b:	6a 01                	push   0x1
  103e3d:	68 ce 01 00 00       	push   0x1ce
  103e42:	e8 e9 05 00 00       	call   104430 <outw>
    outw(VBE_DISPI_IOPORT_DATA, DataValue);
  103e47:	5e                   	pop    esi
  103e48:	58                   	pop    eax
  103e49:	68 00 05 00 00       	push   0x500
  103e4e:	68 cf 01 00 00       	push   0x1cf
  103e53:	e8 d8 05 00 00       	call   104430 <outw>
    outw(VBE_DISPI_IOPORT_INDEX, IndexValue);
  103e58:	58                   	pop    eax
  103e59:	5a                   	pop    edx
  103e5a:	6a 02                	push   0x2
  103e5c:	68 ce 01 00 00       	push   0x1ce
  103e61:	e8 ca 05 00 00       	call   104430 <outw>
    outw(VBE_DISPI_IOPORT_DATA, DataValue);
  103e66:	59                   	pop    ecx
  103e67:	5b                   	pop    ebx
  103e68:	68 d0 02 00 00       	push   0x2d0
  103e6d:	68 cf 01 00 00       	push   0x1cf
  103e72:	e8 b9 05 00 00       	call   104430 <outw>
    outw(VBE_DISPI_IOPORT_INDEX, IndexValue);
  103e77:	5e                   	pop    esi
  103e78:	58                   	pop    eax
  103e79:	6a 03                	push   0x3
  103e7b:	68 ce 01 00 00       	push   0x1ce
  103e80:	e8 ab 05 00 00       	call   104430 <outw>
    outw(VBE_DISPI_IOPORT_DATA, DataValue);
  103e85:	58                   	pop    eax
  103e86:	5a                   	pop    edx
  103e87:	6a 20                	push   0x20
  103e89:	68 cf 01 00 00       	push   0x1cf
  103e8e:	e8 9d 05 00 00       	call   104430 <outw>
    outw(VBE_DISPI_IOPORT_INDEX, IndexValue);
  103e93:	59                   	pop    ecx
  103e94:	5b                   	pop    ebx
  103e95:	6a 04                	push   0x4
  103e97:	68 ce 01 00 00       	push   0x1ce
  103e9c:	e8 8f 05 00 00       	call   104430 <outw>
    outw(VBE_DISPI_IOPORT_DATA, DataValue);
  103ea1:	5e                   	pop    esi
  103ea2:	58                   	pop    eax
  103ea3:	6a 41                	push   0x41
  103ea5:	68 cf 01 00 00       	push   0x1cf
  103eaa:	e8 81 05 00 00       	call   104430 <outw>
    outw(VBE_DISPI_IOPORT_INDEX, IndexValue);
  103eaf:	58                   	pop    eax
  103eb0:	5a                   	pop    edx
  103eb1:	6a 05                	push   0x5
  103eb3:	68 ce 01 00 00       	push   0x1ce
  103eb8:	e8 73 05 00 00       	call   104430 <outw>
    outw(VBE_DISPI_IOPORT_DATA, DataValue);
  103ebd:	59                   	pop    ecx
  103ebe:	5b                   	pop    ebx
  103ebf:	6a 00                	push   0x0
  103ec1:	68 cf 01 00 00       	push   0x1cf
    for (u32 fb_offset = 0xE0000000; fb_offset < 0xFF000000; fb_offset += 0x01000000)
  103ec6:	bb 00 00 00 e0       	mov    ebx,0xe0000000
    outw(VBE_DISPI_IOPORT_DATA, DataValue);
  103ecb:	e8 60 05 00 00       	call   104430 <outw>
    text_vid_mem[0] = 0xA5ADFACE;
  103ed0:	c7 05 00 00 0a 00 ce 	mov    DWORD PTR ds:0xa0000,0xa5adface
  103ed7:	fa ad a5 
  103eda:	83 c4 10             	add    esp,0x10
    u32 * lfb = 0;
  103edd:	31 d2                	xor    edx,edx
  103edf:	90                   	nop
  103ee0:	8d 8b 00 00 ff 00    	lea    ecx,[ebx+0xff0000]
        for (u32 x = fb_offset; x < fb_offset + 0xFF0000; x += 0x1000)
  103ee6:	39 d9                	cmp    ecx,ebx
  103ee8:	76 18                	jbe    103f02 <bga_setup+0x162>
  103eea:	89 d8                	mov    eax,ebx
  103eec:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
            if (((u32 *)x)[0] == 0xA5ADFACE)
  103ef0:	81 38 ce fa ad a5    	cmp    DWORD PTR [eax],0xa5adface
  103ef6:	0f 44 d0             	cmove  edx,eax
        for (u32 x = fb_offset; x < fb_offset + 0xFF0000; x += 0x1000)
  103ef9:	05 00 10 00 00       	add    eax,0x1000
  103efe:	39 c8                	cmp    eax,ecx
  103f00:	72 ee                	jb     103ef0 <bga_setup+0x150>
    for (u32 fb_offset = 0xE0000000; fb_offset < 0xFF000000; fb_offset += 0x01000000)
  103f02:	81 c3 00 00 00 01    	add    ebx,0x1000000
  103f08:	81 fb 00 00 00 ff    	cmp    ebx,0xff000000
  103f0e:	75 d0                	jne    103ee0 <bga_setup+0x140>
        bga_framebuffer = bga_get_framebuffer();
  103f10:	89 15 20 ae 23 00    	mov    DWORD PTR ds:0x23ae20,edx
}
  103f16:	83 c4 04             	add    esp,0x4
  103f19:	5b                   	pop    ebx
  103f1a:	5e                   	pop    esi
  103f1b:	c3                   	ret    
  103f1c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]

00103f20 <bga_mode>:
{
  103f20:	56                   	push   esi
  103f21:	53                   	push   ebx
  103f22:	83 ec 0c             	sub    esp,0xc
  103f25:	8b 74 24 18          	mov    esi,DWORD PTR [esp+0x18]
  103f29:	8b 5c 24 1c          	mov    ebx,DWORD PTR [esp+0x1c]
    outw(VBE_DISPI_IOPORT_INDEX, IndexValue);
  103f2d:	6a 04                	push   0x4
  103f2f:	68 ce 01 00 00       	push   0x1ce
    bga_width = width;
  103f34:	89 35 28 ae 23 00    	mov    DWORD PTR ds:0x23ae28,esi
    bga_height = height;
  103f3a:	89 1d 24 ae 23 00    	mov    DWORD PTR ds:0x23ae24,ebx

    bga_write_register(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
    bga_write_register(VBE_DISPI_INDEX_XRES, width);
  103f40:	0f b7 f6             	movzx  esi,si
    outw(VBE_DISPI_IOPORT_INDEX, IndexValue);
  103f43:	e8 e8 04 00 00       	call   104430 <outw>
    outw(VBE_DISPI_IOPORT_DATA, DataValue);
  103f48:	58                   	pop    eax
  103f49:	5a                   	pop    edx
  103f4a:	6a 00                	push   0x0
  103f4c:	68 cf 01 00 00       	push   0x1cf
    bga_write_register(VBE_DISPI_INDEX_YRES, height);
  103f51:	0f b7 db             	movzx  ebx,bx
    outw(VBE_DISPI_IOPORT_DATA, DataValue);
  103f54:	e8 d7 04 00 00       	call   104430 <outw>
    outw(VBE_DISPI_IOPORT_INDEX, IndexValue);
  103f59:	59                   	pop    ecx
  103f5a:	58                   	pop    eax
  103f5b:	6a 01                	push   0x1
  103f5d:	68 ce 01 00 00       	push   0x1ce
  103f62:	e8 c9 04 00 00       	call   104430 <outw>
    outw(VBE_DISPI_IOPORT_DATA, DataValue);
  103f67:	58                   	pop    eax
  103f68:	5a                   	pop    edx
  103f69:	56                   	push   esi
  103f6a:	68 cf 01 00 00       	push   0x1cf
  103f6f:	e8 bc 04 00 00       	call   104430 <outw>
    outw(VBE_DISPI_IOPORT_INDEX, IndexValue);
  103f74:	59                   	pop    ecx
  103f75:	5e                   	pop    esi
  103f76:	6a 02                	push   0x2
  103f78:	68 ce 01 00 00       	push   0x1ce
  103f7d:	e8 ae 04 00 00       	call   104430 <outw>
    outw(VBE_DISPI_IOPORT_DATA, DataValue);
  103f82:	58                   	pop    eax
  103f83:	5a                   	pop    edx
  103f84:	53                   	push   ebx
  103f85:	68 cf 01 00 00       	push   0x1cf
  103f8a:	e8 a1 04 00 00       	call   104430 <outw>
    outw(VBE_DISPI_IOPORT_INDEX, IndexValue);
  103f8f:	59                   	pop    ecx
  103f90:	5b                   	pop    ebx
  103f91:	6a 03                	push   0x3
  103f93:	68 ce 01 00 00       	push   0x1ce
  103f98:	e8 93 04 00 00       	call   104430 <outw>
    outw(VBE_DISPI_IOPORT_DATA, DataValue);
  103f9d:	5e                   	pop    esi
  103f9e:	58                   	pop    eax
  103f9f:	6a 20                	push   0x20
  103fa1:	68 cf 01 00 00       	push   0x1cf
  103fa6:	e8 85 04 00 00       	call   104430 <outw>
    outw(VBE_DISPI_IOPORT_INDEX, IndexValue);
  103fab:	58                   	pop    eax
  103fac:	5a                   	pop    edx
  103fad:	6a 04                	push   0x4
  103faf:	68 ce 01 00 00       	push   0x1ce
  103fb4:	e8 77 04 00 00       	call   104430 <outw>
    outw(VBE_DISPI_IOPORT_DATA, DataValue);
  103fb9:	c7 44 24 24 41 00 00 	mov    DWORD PTR [esp+0x24],0x41
  103fc0:	00 
  103fc1:	c7 44 24 20 cf 01 00 	mov    DWORD PTR [esp+0x20],0x1cf
  103fc8:	00 
    bga_write_register(VBE_DISPI_INDEX_BPP, 32);
    bga_write_register(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED);
}
  103fc9:	83 c4 14             	add    esp,0x14
  103fcc:	5b                   	pop    ebx
  103fcd:	5e                   	pop    esi
    outw(VBE_DISPI_IOPORT_DATA, DataValue);
  103fce:	e9 5d 04 00 00       	jmp    104430 <outw>
  103fd3:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
  103fd9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00103fe0 <bga_blit>:

void bga_blit(buffer32_t buffer)
{ 
    for(u32 x = 0; x < bga_width; x++)
  103fe0:	a1 28 ae 23 00       	mov    eax,ds:0x23ae28
{ 
  103fe5:	53                   	push   ebx
  103fe6:	8b 4c 24 08          	mov    ecx,DWORD PTR [esp+0x8]
    for(u32 x = 0; x < bga_width; x++)
  103fea:	85 c0                	test   eax,eax
  103fec:	74 25                	je     104013 <bga_blit+0x33>
    {
        for(u32 y = 0; y < bga_height; x++)
  103fee:	a1 24 ae 23 00       	mov    eax,ds:0x23ae24
        {
            bga_framebuffer[x + y * bga_width] = buffer[x + y * bga_width];
  103ff3:	8b 1d 20 ae 23 00    	mov    ebx,DWORD PTR ds:0x23ae20
        for(u32 y = 0; y < bga_height; x++)
  103ff9:	85 c0                	test   eax,eax
  103ffb:	74 16                	je     104013 <bga_blit+0x33>
    for(u32 x = 0; x < bga_width; x++)
  103ffd:	31 c0                	xor    eax,eax
  103fff:	90                   	nop
            bga_framebuffer[x + y * bga_width] = buffer[x + y * bga_width];
  104000:	8b 14 81             	mov    edx,DWORD PTR [ecx+eax*4]
  104003:	89 14 83             	mov    DWORD PTR [ebx+eax*4],edx
        for(u32 y = 0; y < bga_height; x++)
  104006:	8b 15 24 ae 23 00    	mov    edx,DWORD PTR ds:0x23ae24
  10400c:	83 c0 01             	add    eax,0x1
  10400f:	85 d2                	test   edx,edx
  104011:	75 ed                	jne    104000 <bga_blit+0x20>
        }
    }
}
  104013:	5b                   	pop    ebx
  104014:	c3                   	ret    
  104015:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
  104019:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00104020 <bga_pixel>:

void bga_pixel(u32 x, u32 y, u32 value)
{
    bga_framebuffer[x + y * bga_width] = value;
  104020:	8b 44 24 08          	mov    eax,DWORD PTR [esp+0x8]
  104024:	8b 4c 24 0c          	mov    ecx,DWORD PTR [esp+0xc]
  104028:	0f af 05 28 ae 23 00 	imul   eax,DWORD PTR ds:0x23ae28
  10402f:	8b 15 20 ae 23 00    	mov    edx,DWORD PTR ds:0x23ae20
  104035:	03 44 24 04          	add    eax,DWORD PTR [esp+0x4]
  104039:	89 0c 82             	mov    DWORD PTR [edx+eax*4],ecx
  10403c:	c3                   	ret    
  10403d:	66 90                	xchg   ax,ax
  10403f:	90                   	nop

00104040 <vga_setup>:
#include "cpu/cpu.h"

u16 *video_memory = (u16 *)vga_frame_buffer;

void vga_setup()
{
  104040:	53                   	push   ebx
  104041:	83 ec 08             	sub    esp,0x8
  104044:	8b 0d 14 70 11 00    	mov    ecx,DWORD PTR ds:0x117014
  10404a:	8d 99 a0 00 00 00    	lea    ebx,[ecx+0xa0]
  104050:	8d 91 40 10 00 00    	lea    edx,[ecx+0x1040]
  104056:	89 c8                	mov    eax,ecx
  104058:	90                   	nop
  104059:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
{
    for (u32 x = 0; x < vga_screen_width; x++)
    {
        for (u32 y = 0; y < vga_screen_height + 1; y++)
        {
            video_memory[y * vga_screen_width + x] = vga_entry(' ', fg, bg);
  104060:	66 c7 00 20 0f       	mov    WORD PTR [eax],0xf20
  104065:	05 a0 00 00 00       	add    eax,0xa0
        for (u32 y = 0; y < vga_screen_height + 1; y++)
  10406a:	39 c2                	cmp    edx,eax
  10406c:	75 f2                	jne    104060 <vga_setup+0x20>
  10406e:	83 c1 02             	add    ecx,0x2
    for (u32 x = 0; x < vga_screen_width; x++)
  104071:	39 d9                	cmp    ecx,ebx
  104073:	75 db                	jne    104050 <vga_setup+0x10>
}

void vga_cursor(s32 x, s32 y)
{
    s16 cursorLocation = y * vga_screen_width + x;
    outb(0x3D4, 14);                  // Tell the VGA board we are setting the high cursor byte.
  104075:	83 ec 08             	sub    esp,0x8
  104078:	6a 0e                	push   0xe
  10407a:	68 d4 03 00 00       	push   0x3d4
  10407f:	e8 8c 03 00 00       	call   104410 <outb>
    outb(0x3D5, cursorLocation >> 8); // Send the high cursor byte.
  104084:	58                   	pop    eax
  104085:	5a                   	pop    edx
  104086:	6a 00                	push   0x0
  104088:	68 d5 03 00 00       	push   0x3d5
  10408d:	e8 7e 03 00 00       	call   104410 <outb>
    outb(0x3D4, 15);                  // Tell the VGA board we are setting the low cursor byte.
  104092:	59                   	pop    ecx
  104093:	5b                   	pop    ebx
  104094:	6a 0f                	push   0xf
  104096:	68 d4 03 00 00       	push   0x3d4
  10409b:	e8 70 03 00 00       	call   104410 <outb>
    outb(0x3D5, cursorLocation);      // Send the low cursor byte.
  1040a0:	58                   	pop    eax
  1040a1:	5a                   	pop    edx
  1040a2:	6a 00                	push   0x0
  1040a4:	68 d5 03 00 00       	push   0x3d5
  1040a9:	e8 62 03 00 00       	call   104410 <outb>
}
  1040ae:	83 c4 18             	add    esp,0x18
  1040b1:	5b                   	pop    ebx
  1040b2:	c3                   	ret    
  1040b3:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
  1040b9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

001040c0 <vga_clear>:
{
  1040c0:	56                   	push   esi
  1040c1:	53                   	push   ebx
            video_memory[y * vga_screen_width + x] = vga_entry(' ', fg, bg);
  1040c2:	0f b6 4c 24 10       	movzx  ecx,BYTE PTR [esp+0x10]
  1040c7:	0f b6 44 24 0c       	movzx  eax,BYTE PTR [esp+0xc]
  1040cc:	8b 1d 14 70 11 00    	mov    ebx,DWORD PTR ds:0x117014
  1040d2:	c1 e1 04             	shl    ecx,0x4
  1040d5:	8d b3 a0 00 00 00    	lea    esi,[ebx+0xa0]
  1040db:	09 c1                	or     ecx,eax
  1040dd:	c1 e1 08             	shl    ecx,0x8
  1040e0:	83 c9 20             	or     ecx,0x20
  1040e3:	90                   	nop
  1040e4:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
  1040e8:	8d 93 40 10 00 00    	lea    edx,[ebx+0x1040]
{
  1040ee:	89 d8                	mov    eax,ebx
            video_memory[y * vga_screen_width + x] = vga_entry(' ', fg, bg);
  1040f0:	66 89 08             	mov    WORD PTR [eax],cx
  1040f3:	05 a0 00 00 00       	add    eax,0xa0
        for (u32 y = 0; y < vga_screen_height + 1; y++)
  1040f8:	39 c2                	cmp    edx,eax
  1040fa:	75 f4                	jne    1040f0 <vga_clear+0x30>
  1040fc:	83 c3 02             	add    ebx,0x2
    for (u32 x = 0; x < vga_screen_width; x++)
  1040ff:	39 f3                	cmp    ebx,esi
  104101:	75 e5                	jne    1040e8 <vga_clear+0x28>
}
  104103:	5b                   	pop    ebx
  104104:	5e                   	pop    esi
  104105:	c3                   	ret    
  104106:	8d 76 00             	lea    esi,[esi+0x0]
  104109:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00104110 <vga_scroll>:
{
  104110:	55                   	push   ebp
  104111:	57                   	push   edi
  104112:	56                   	push   esi
  104113:	53                   	push   ebx
  104114:	83 ec 04             	sub    esp,0x4
            video_memory[y * vga_screen_width + x] = video_memory[(y + 1) * vga_screen_width + x];
  104117:	8b 0d 14 70 11 00    	mov    ecx,DWORD PTR ds:0x117014
{
  10411d:	8b 6c 24 18          	mov    ebp,DWORD PTR [esp+0x18]
  104121:	8d b9 a0 00 00 00    	lea    edi,[ecx+0xa0]
  104127:	89 ce                	mov    esi,ecx
  104129:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
  104130:	8d 9e 40 10 00 00    	lea    ebx,[esi+0x1040]
  104136:	89 f0                	mov    eax,esi
  104138:	90                   	nop
  104139:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
            video_memory[y * vga_screen_width + x] = video_memory[(y + 1) * vga_screen_width + x];
  104140:	0f b7 90 a0 00 00 00 	movzx  edx,WORD PTR [eax+0xa0]
  104147:	05 a0 00 00 00       	add    eax,0xa0
  10414c:	66 89 90 60 ff ff ff 	mov    WORD PTR [eax-0xa0],dx
        for (u32 y = 0; y < vga_screen_height + 1; y++)
  104153:	39 d8                	cmp    eax,ebx
  104155:	75 e9                	jne    104140 <vga_scroll+0x30>
  104157:	83 c6 02             	add    esi,0x2
    for (u32 x = 0; x < vga_screen_width; x++)
  10415a:	39 fe                	cmp    esi,edi
  10415c:	75 d2                	jne    104130 <vga_scroll+0x20>
    u16 entry = vga_entry(' ', vga_black, bg);
  10415e:	89 ea                	mov    edx,ebp
  104160:	89 c8                	mov    eax,ecx
    for (u32 i = 0; i < vga_screen_width; i++)
  104162:	c7 04 24 00 00 00 00 	mov    DWORD PTR [esp],0x0
    u16 entry = vga_entry(' ', vga_black, bg);
  104169:	c1 e2 0c             	shl    edx,0xc
  10416c:	d1 e8                	shr    eax,1
  10416e:	83 ca 20             	or     edx,0x20
  104171:	83 e0 01             	and    eax,0x1
  104174:	74 0e                	je     104184 <vga_scroll+0x74>
        video_memory[((vga_screen_height - 1) * vga_screen_width) + i] = entry;
  104176:	66 89 91 00 0f 00 00 	mov    WORD PTR [ecx+0xf00],dx
    for (u32 i = 0; i < vga_screen_width; i++)
  10417d:	c7 04 24 01 00 00 00 	mov    DWORD PTR [esp],0x1
  104184:	bd 50 00 00 00       	mov    ebp,0x50
  104189:	89 d3                	mov    ebx,edx
  10418b:	0f b7 f2             	movzx  esi,dx
  10418e:	29 c5                	sub    ebp,eax
  104190:	c1 e3 10             	shl    ebx,0x10
  104193:	05 80 07 00 00       	add    eax,0x780
  104198:	89 ef                	mov    edi,ebp
  10419a:	09 f3                	or     ebx,esi
  10419c:	8d 34 41             	lea    esi,[ecx+eax*2]
  10419f:	d1 ef                	shr    edi,1
  1041a1:	31 c0                	xor    eax,eax
  1041a3:	90                   	nop
  1041a4:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
        video_memory[((vga_screen_height - 1) * vga_screen_width) + i] = entry;
  1041a8:	89 1c 86             	mov    DWORD PTR [esi+eax*4],ebx
  1041ab:	83 c0 01             	add    eax,0x1
  1041ae:	39 c7                	cmp    edi,eax
  1041b0:	77 f6                	ja     1041a8 <vga_scroll+0x98>
  1041b2:	8b 04 24             	mov    eax,DWORD PTR [esp]
  1041b5:	89 eb                	mov    ebx,ebp
  1041b7:	83 e3 fe             	and    ebx,0xfffffffe
  1041ba:	01 d8                	add    eax,ebx
  1041bc:	39 dd                	cmp    ebp,ebx
  1041be:	74 08                	je     1041c8 <vga_scroll+0xb8>
  1041c0:	66 89 94 41 00 0f 00 	mov    WORD PTR [ecx+eax*2+0xf00],dx
  1041c7:	00 
}
  1041c8:	83 c4 04             	add    esp,0x4
  1041cb:	5b                   	pop    ebx
  1041cc:	5e                   	pop    esi
  1041cd:	5f                   	pop    edi
  1041ce:	5d                   	pop    ebp
  1041cf:	c3                   	ret    

001041d0 <vga_cell>:
{
  1041d0:	8b 44 24 08          	mov    eax,DWORD PTR [esp+0x8]
  1041d4:	8b 54 24 0c          	mov    edx,DWORD PTR [esp+0xc]
        if (y < vga_screen_height)
  1041d8:	83 f8 18             	cmp    eax,0x18
  1041db:	77 07                	ja     1041e4 <vga_cell+0x14>
  1041dd:	83 7c 24 04 4f       	cmp    DWORD PTR [esp+0x4],0x4f
  1041e2:	76 0c                	jbe    1041f0 <vga_cell+0x20>
}
  1041e4:	f3 c3                	repz ret 
  1041e6:	8d 76 00             	lea    esi,[esi+0x0]
  1041e9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
            video_memory[y * vga_screen_width + x] = (u16)entry;
  1041f0:	8d 04 80             	lea    eax,[eax+eax*4]
  1041f3:	8b 0d 14 70 11 00    	mov    ecx,DWORD PTR ds:0x117014
  1041f9:	c1 e0 04             	shl    eax,0x4
  1041fc:	03 44 24 04          	add    eax,DWORD PTR [esp+0x4]
  104200:	66 89 14 41          	mov    WORD PTR [ecx+eax*2],dx
}
  104204:	c3                   	ret    
  104205:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
  104209:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00104210 <vga_text>:
{
  104210:	57                   	push   edi
  104211:	56                   	push   esi
  104212:	53                   	push   ebx
  104213:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
  104217:	8b 74 24 14          	mov    esi,DWORD PTR [esp+0x14]
  10421b:	8b 5c 24 1c          	mov    ebx,DWORD PTR [esp+0x1c]
  10421f:	8b 44 24 20          	mov    eax,DWORD PTR [esp+0x20]
    for(size_t i = 0; text[i]; i++)
  104223:	66 0f be 11          	movsx  dx,BYTE PTR [ecx]
  104227:	84 d2                	test   dl,dl
  104229:	74 3a                	je     104265 <vga_text+0x55>
        vga_cell(x + i, y, vga_entry(text[i], fg, bg));
  10422b:	0f b6 c0             	movzx  eax,al
  10422e:	c1 e0 04             	shl    eax,0x4
  104231:	09 c3                	or     ebx,eax
            video_memory[y * vga_screen_width + x] = (u16)entry;
  104233:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
        vga_cell(x + i, y, vga_entry(text[i], fg, bg));
  104237:	89 df                	mov    edi,ebx
  104239:	c1 e7 08             	shl    edi,0x8
  10423c:	83 fe 18             	cmp    esi,0x18
  10423f:	8d 34 b6             	lea    esi,[esi+esi*4]
  104242:	0f 96 c3             	setbe  bl
    for(size_t i = 0; text[i]; i++)
  104245:	29 c1                	sub    ecx,eax
  104247:	c1 e6 05             	shl    esi,0x5
  10424a:	03 35 14 70 11 00    	add    esi,DWORD PTR ds:0x117014
        if (y < vga_screen_height)
  104250:	83 f8 4f             	cmp    eax,0x4f
  104253:	77 04                	ja     104259 <vga_text+0x49>
  104255:	84 db                	test   bl,bl
  104257:	75 17                	jne    104270 <vga_text+0x60>
  104259:	83 c0 01             	add    eax,0x1
    for(size_t i = 0; text[i]; i++)
  10425c:	66 0f be 14 01       	movsx  dx,BYTE PTR [ecx+eax*1]
  104261:	84 d2                	test   dl,dl
  104263:	75 eb                	jne    104250 <vga_text+0x40>
}
  104265:	5b                   	pop    ebx
  104266:	5e                   	pop    esi
  104267:	5f                   	pop    edi
  104268:	c3                   	ret    
  104269:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
        vga_cell(x + i, y, vga_entry(text[i], fg, bg));
  104270:	09 fa                	or     edx,edi
  104272:	66 89 14 46          	mov    WORD PTR [esi+eax*2],dx
  104276:	83 c0 01             	add    eax,0x1
    for(size_t i = 0; text[i]; i++)
  104279:	66 0f be 14 01       	movsx  dx,BYTE PTR [ecx+eax*1]
  10427e:	84 d2                	test   dl,dl
  104280:	75 ce                	jne    104250 <vga_text+0x40>
}
  104282:	5b                   	pop    ebx
  104283:	5e                   	pop    esi
  104284:	5f                   	pop    edi
  104285:	c3                   	ret    
  104286:	8d 76 00             	lea    esi,[esi+0x0]
  104289:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00104290 <vga_cursor>:
{
  104290:	53                   	push   ebx
  104291:	83 ec 10             	sub    esp,0x10
  104294:	8b 5c 24 1c          	mov    ebx,DWORD PTR [esp+0x1c]
    s16 cursorLocation = y * vga_screen_width + x;
  104298:	8d 1c 9b             	lea    ebx,[ebx+ebx*4]
  10429b:	c1 e3 04             	shl    ebx,0x4
  10429e:	66 03 5c 24 18       	add    bx,WORD PTR [esp+0x18]
    outb(0x3D4, 14);                  // Tell the VGA board we are setting the high cursor byte.
  1042a3:	6a 0e                	push   0xe
  1042a5:	68 d4 03 00 00       	push   0x3d4
  1042aa:	e8 61 01 00 00       	call   104410 <outb>
    outb(0x3D5, cursorLocation >> 8); // Send the high cursor byte.
  1042af:	58                   	pop    eax
  1042b0:	5a                   	pop    edx
  1042b1:	0f b6 c7             	movzx  eax,bh
    outb(0x3D5, cursorLocation);      // Send the low cursor byte.
  1042b4:	0f b6 db             	movzx  ebx,bl
    outb(0x3D5, cursorLocation >> 8); // Send the high cursor byte.
  1042b7:	50                   	push   eax
  1042b8:	68 d5 03 00 00       	push   0x3d5
  1042bd:	e8 4e 01 00 00       	call   104410 <outb>
    outb(0x3D4, 15);                  // Tell the VGA board we are setting the low cursor byte.
  1042c2:	59                   	pop    ecx
  1042c3:	58                   	pop    eax
  1042c4:	6a 0f                	push   0xf
  1042c6:	68 d4 03 00 00       	push   0x3d4
  1042cb:	e8 40 01 00 00       	call   104410 <outb>
    outb(0x3D5, cursorLocation);      // Send the low cursor byte.
  1042d0:	89 5c 24 24          	mov    DWORD PTR [esp+0x24],ebx
  1042d4:	c7 44 24 20 d5 03 00 	mov    DWORD PTR [esp+0x20],0x3d5
  1042db:	00 
}
  1042dc:	83 c4 18             	add    esp,0x18
  1042df:	5b                   	pop    ebx
    outb(0x3D5, cursorLocation);      // Send the low cursor byte.
  1042e0:	e9 2b 01 00 00       	jmp    104410 <outb>
  1042e5:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
  1042e9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

001042f0 <vga_hide_cursor>:

void vga_hide_cursor()
{
  1042f0:	83 ec 14             	sub    esp,0x14
    outb(0x3D4, 14);                  // Tell the VGA board we are setting the high cursor byte.
  1042f3:	6a 0e                	push   0xe
  1042f5:	68 d4 03 00 00       	push   0x3d4
  1042fa:	e8 11 01 00 00       	call   104410 <outb>
    outb(0x3D5, cursorLocation >> 8); // Send the high cursor byte.
  1042ff:	58                   	pop    eax
  104300:	5a                   	pop    edx
  104301:	68 ff 00 00 00       	push   0xff
  104306:	68 d5 03 00 00       	push   0x3d5
  10430b:	e8 00 01 00 00       	call   104410 <outb>
    outb(0x3D4, 15);                  // Tell the VGA board we are setting the low cursor byte.
  104310:	59                   	pop    ecx
  104311:	58                   	pop    eax
  104312:	6a 0f                	push   0xf
  104314:	68 d4 03 00 00       	push   0x3d4
  104319:	e8 f2 00 00 00       	call   104410 <outb>
    outb(0x3D5, cursorLocation);      // Send the low cursor byte.
  10431e:	58                   	pop    eax
  10431f:	5a                   	pop    edx
  104320:	68 af 00 00 00       	push   0xaf
  104325:	68 d5 03 00 00       	push   0x3d5
  10432a:	e8 e1 00 00 00       	call   104410 <outb>
    vga_cursor(-1, -1);
}
  10432f:	83 c4 1c             	add    esp,0x1c
  104332:	c3                   	ret    
  104333:	66 90                	xchg   ax,ax
  104335:	66 90                	xchg   ax,ax
  104337:	66 90                	xchg   ax,ax
  104339:	66 90                	xchg   ax,ax
  10433b:	66 90                	xchg   ax,ax
  10433d:	66 90                	xchg   ax,ax
  10433f:	90                   	nop

00104340 <irq_setup>:
extern u32 irq_vector[];
extern bool console_bypass_lock;
irq_handler_t irq_handlers[16];

void irq_setup()
{
  104340:	53                   	push   ebx
  104341:	bb 20 00 00 00       	mov    ebx,0x20
  104346:	83 ec 08             	sub    esp,0x8
  104349:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
    for(u32 i = 0; i < 16; i++)
    {
        idt_entry(32 + i, irq_vector[i], 0x08, INTGATE);
  104350:	68 8e 00 00 00       	push   0x8e
  104355:	6a 08                	push   0x8
  104357:	ff 34 9d 85 51 10 00 	push   DWORD PTR [ebx*4+0x105185]
  10435e:	53                   	push   ebx
  10435f:	83 c3 01             	add    ebx,0x1
  104362:	e8 f9 07 00 00       	call   104b60 <idt_entry>
    for(u32 i = 0; i < 16; i++)
  104367:	83 c4 10             	add    esp,0x10
  10436a:	83 fb 30             	cmp    ebx,0x30
  10436d:	75 e1                	jne    104350 <irq_setup+0x10>
    }   
}
  10436f:	83 c4 08             	add    esp,0x8
  104372:	5b                   	pop    ebx
  104373:	c3                   	ret    
  104374:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
  10437a:	8d bf 00 00 00 00    	lea    edi,[edi+0x0]

00104380 <irq_register>:

irq_handler_t irq_register(int index, irq_handler_t handler)
{
  104380:	8b 54 24 04          	mov    edx,DWORD PTR [esp+0x4]
  104384:	8b 44 24 08          	mov    eax,DWORD PTR [esp+0x8]
    if (index < 16)
  104388:	83 fa 0f             	cmp    edx,0xf
  10438b:	7f 0b                	jg     104398 <irq_register+0x18>
    {
        irq_handlers[index] = handler;
  10438d:	89 04 95 80 a0 23 00 	mov    DWORD PTR [edx*4+0x23a080],eax
        return handler;
  104394:	c3                   	ret    
  104395:	8d 76 00             	lea    esi,[esi+0x0]
    }

    return NULL;
  104398:	31 c0                	xor    eax,eax
}
  10439a:	c3                   	ret    
  10439b:	90                   	nop
  10439c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]

001043a0 <irq_handler>:

esp_t irq_handler(esp_t esp, context_t context)
{
  1043a0:	53                   	push   ebx
  1043a1:	83 ec 08             	sub    esp,0x8
    if (irq_handlers[context.int_no] != NULL)
  1043a4:	8b 44 24 44          	mov    eax,DWORD PTR [esp+0x44]
  1043a8:	8b 14 85 80 a0 23 00 	mov    edx,DWORD PTR [eax*4+0x23a080]
  1043af:	85 d2                	test   edx,edx
  1043b1:	74 45                	je     1043f8 <irq_handler+0x58>
    {
        esp = irq_handlers[context.int_no](esp, &context);
  1043b3:	83 ec 08             	sub    esp,0x8
  1043b6:	8d 44 24 1c          	lea    eax,[esp+0x1c]
  1043ba:	50                   	push   eax
  1043bb:	ff 74 24 1c          	push   DWORD PTR [esp+0x1c]
  1043bf:	ff d2                	call   edx
  1043c1:	89 c3                	mov    ebx,eax
  1043c3:	8b 44 24 54          	mov    eax,DWORD PTR [esp+0x54]
  1043c7:	83 c4 10             	add    esp,0x10
        // console_bypass_lock = true;
        // debug("Unhandeled IRQ %d!", context.int_no);
        // console_bypass_lock = false;
    }

    if (context.int_no >= 8)
  1043ca:	83 f8 07             	cmp    eax,0x7
  1043cd:	76 12                	jbe    1043e1 <irq_handler+0x41>
    {
        outb(0xA0, 0x20);
  1043cf:	83 ec 08             	sub    esp,0x8
  1043d2:	6a 20                	push   0x20
  1043d4:	68 a0 00 00 00       	push   0xa0
  1043d9:	e8 32 00 00 00       	call   104410 <outb>
  1043de:	83 c4 10             	add    esp,0x10
    }

    outb(0x20, 0x20);
  1043e1:	83 ec 08             	sub    esp,0x8
  1043e4:	6a 20                	push   0x20
  1043e6:	6a 20                	push   0x20
  1043e8:	e8 23 00 00 00       	call   104410 <outb>

    // this is only use for task switching.
    return esp;
  1043ed:	83 c4 18             	add    esp,0x18
  1043f0:	89 d8                	mov    eax,ebx
  1043f2:	5b                   	pop    ebx
  1043f3:	c3                   	ret    
  1043f4:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
  1043f8:	8b 5c 24 10          	mov    ebx,DWORD PTR [esp+0x10]
  1043fc:	eb cc                	jmp    1043ca <irq_handler+0x2a>
  1043fe:	66 90                	xchg   ax,ax

00104400 <inb>:
#include "cpu/cpu.h"

u8 inb(u16 port)
{
    u8 data;
    asm volatile("in %1,%0" : "=a" (data) : "d" (port));
  104400:	8b 54 24 04          	mov    edx,DWORD PTR [esp+0x4]
  104404:	ec                   	in     al,dx
    return data;
}
  104405:	c3                   	ret    
  104406:	8d 76 00             	lea    esi,[esi+0x0]
  104409:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00104410 <outb>:

void outb(u16 port, u8 data)
{
    asm volatile("out %0,%1" : : "a" (data), "d" (port));
  104410:	8b 54 24 04          	mov    edx,DWORD PTR [esp+0x4]
  104414:	8b 44 24 08          	mov    eax,DWORD PTR [esp+0x8]
  104418:	ee                   	out    dx,al
}
  104419:	c3                   	ret    
  10441a:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]

00104420 <inw>:

u16 inw(u16 port)
{
    u16 data;
    asm volatile("inw %1,%0" : "=a" (data) : "d" (port));
  104420:	8b 54 24 04          	mov    edx,DWORD PTR [esp+0x4]
  104424:	66 ed                	in     ax,dx
    return data;
}
  104426:	c3                   	ret    
  104427:	89 f6                	mov    esi,esi
  104429:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00104430 <outw>:

void outw(u16 port, u16 data)
{
    asm volatile("outw %0,%1" : : "a" (data), "d" (port));
  104430:	8b 54 24 04          	mov    edx,DWORD PTR [esp+0x4]
  104434:	8b 44 24 08          	mov    eax,DWORD PTR [esp+0x8]
  104438:	66 ef                	out    dx,ax
}
  10443a:	c3                   	ret    
  10443b:	90                   	nop
  10443c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]

00104440 <CR0>:

u32 CR0()
{
    u32 r;
    asm volatile("mov %%cr0, %0" : "=r" (r));
  104440:	0f 20 c0             	mov    eax,cr0
    return r;
}
  104443:	c3                   	ret    
  104444:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
  10444a:	8d bf 00 00 00 00    	lea    edi,[edi+0x0]

00104450 <CR2>:

u32 CR2()
{
    u32 r;
    asm volatile("mov %%cr2, %0" : "=r" (r));
  104450:	0f 20 d0             	mov    eax,cr2
    return r;
}
  104453:	c3                   	ret    
  104454:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
  10445a:	8d bf 00 00 00 00    	lea    edi,[edi+0x0]

00104460 <CR3>:

u32 CR3()
{
    u32 r;
    asm volatile("mov %%cr3, %0" : "=r" (r));
  104460:	0f 20 d8             	mov    eax,cr3
    return r;
}
  104463:	c3                   	ret    
  104464:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
  10446a:	8d bf 00 00 00 00    	lea    edi,[edi+0x0]

00104470 <CR4>:

u32 CR4()
{
    u32 r;
    asm volatile("mov %%cr4, %0" : "=r" (r));
  104470:	0f 20 e0             	mov    eax,cr4
    return r;
}
  104473:	c3                   	ret    
  104474:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
  10447a:	8d bf 00 00 00 00    	lea    edi,[edi+0x0]

00104480 <dump_context>:

void dump_context(context_t* context)
{
  104480:	53                   	push   ebx
  104481:	83 ec 10             	sub    esp,0x10
  104484:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
    printf("\tGS=%x FS=%x ES=%x DS=%x \n\tEDI=%x ESI=%x EBP=%x USELESS=%x \n\tEAX=%x EBX=%x ECX=%x EDX=%x\n\tINTNO=%x ERRCODE=%x EIP=%x CS=%x FLAGS=%x \n",
  104488:	ff 70 40             	push   DWORD PTR [eax+0x40]
  10448b:	ff 70 3c             	push   DWORD PTR [eax+0x3c]
  10448e:	ff 70 38             	push   DWORD PTR [eax+0x38]
  104491:	ff 70 34             	push   DWORD PTR [eax+0x34]
  104494:	ff 70 30             	push   DWORD PTR [eax+0x30]
  104497:	ff 70 24             	push   DWORD PTR [eax+0x24]
  10449a:	ff 70 28             	push   DWORD PTR [eax+0x28]
  10449d:	ff 70 20             	push   DWORD PTR [eax+0x20]
  1044a0:	ff 70 2c             	push   DWORD PTR [eax+0x2c]
  1044a3:	ff 70 1c             	push   DWORD PTR [eax+0x1c]
  1044a6:	ff 70 18             	push   DWORD PTR [eax+0x18]
  1044a9:	ff 70 14             	push   DWORD PTR [eax+0x14]
  1044ac:	ff 70 10             	push   DWORD PTR [eax+0x10]
  1044af:	ff 70 0c             	push   DWORD PTR [eax+0xc]
  1044b2:	ff 70 08             	push   DWORD PTR [eax+0x8]
  1044b5:	ff 70 04             	push   DWORD PTR [eax+0x4]
  1044b8:	ff 30                	push   DWORD PTR [eax]
  1044ba:	68 38 67 11 00       	push   0x116738
  1044bf:	e8 ac 21 00 00       	call   106670 <printf>
    asm volatile("mov %%cr4, %0" : "=r" (r));
  1044c4:	0f 20 e3             	mov    ebx,cr4
    asm volatile("mov %%cr3, %0" : "=r" (r));
  1044c7:	0f 20 d9             	mov    ecx,cr3
    asm volatile("mov %%cr2, %0" : "=r" (r));
  1044ca:	0f 20 d2             	mov    edx,cr2
    asm volatile("mov %%cr0, %0" : "=r" (r));
  1044cd:	0f 20 c0             	mov    eax,cr0
    context->gs , context->fs , context->es , context->ds , context->edi, context->esi, context->ebp, context->USELESS, context->eax, context->ebx, context->ecx, context->edx, context->int_no, context->errcode, context->eip, context->cs , context->eflags);

    printf("\tCR0=%x CR2=%x CR3=%x CR4=%x\n", CR0(), CR2(), CR3(), CR4());
  1044d0:	83 c4 44             	add    esp,0x44
  1044d3:	53                   	push   ebx
  1044d4:	51                   	push   ecx
  1044d5:	52                   	push   edx
  1044d6:	50                   	push   eax
  1044d7:	68 e0 22 11 00       	push   0x1122e0
  1044dc:	e8 8f 21 00 00       	call   106670 <printf>
}
  1044e1:	83 c4 28             	add    esp,0x28
  1044e4:	5b                   	pop    ebx
  1044e5:	c3                   	ret    
  1044e6:	66 90                	xchg   ax,ax
  1044e8:	66 90                	xchg   ax,ax
  1044ea:	66 90                	xchg   ax,ax
  1044ec:	66 90                	xchg   ax,ax
  1044ee:	66 90                	xchg   ax,ax

001044f0 <cpuid_get_vendorid>:
                "=d"(*(where+1)),"=c"(*(where+2)):"a"(code));
    return (int)where[0];
}
 
string cpuid_get_vendorid() 
{
  1044f0:	53                   	push   ebx
    asm volatile ("cpuid":"=a"(*where),"=b"(*(where+0)),
  1044f1:	31 c0                	xor    eax,eax
  1044f3:	0f a2                	cpuid  
    static char s[16] = "BogusProces!";
    cpuid_string(0, (int*)(s));
    return s;
}
  1044f5:	b8 18 70 11 00       	mov    eax,0x117018
    asm volatile ("cpuid":"=a"(*where),"=b"(*(where+0)),
  1044fa:	89 1d 18 70 11 00    	mov    DWORD PTR ds:0x117018,ebx
  104500:	89 15 1c 70 11 00    	mov    DWORD PTR ds:0x11701c,edx
  104506:	89 0d 20 70 11 00    	mov    DWORD PTR ds:0x117020,ecx
}
  10450c:	5b                   	pop    ebx
  10450d:	c3                   	ret    
  10450e:	66 90                	xchg   ax,ax

00104510 <cpuid>:

cpuid_t cpuid()
{
  104510:	56                   	push   esi
  104511:	53                   	push   ebx
  104512:	83 ec 28             	sub    esp,0x28
  104515:	8b 74 24 34          	mov    esi,DWORD PTR [esp+0x34]
    cpuid_t cid;
    memset(&cid.vendorid, 0, 16);
  104519:	6a 10                	push   0x10
  10451b:	6a 00                	push   0x0
  10451d:	8d 44 24 14          	lea    eax,[esp+0x14]
  104521:	50                   	push   eax
  104522:	e8 a9 11 00 00       	call   1056d0 <memset>
    asm volatile ("cpuid":"=a"(*where),"=b"(*(where+0)),
  104527:	31 c0                	xor    eax,eax
  104529:	0f a2                	cpuid  
  10452b:	89 54 24 1c          	mov    DWORD PTR [esp+0x1c],edx
  10452f:	89 5c 24 18          	mov    DWORD PTR [esp+0x18],ebx
  104533:	89 4c 24 20          	mov    DWORD PTR [esp+0x20],ecx
    cpuid_string(0, (int*)&cid.vendorid[0]);
    cid.RAW_ECX = cpuid_get_feature_ECX();
  104537:	e8 1e 0f 00 00       	call   10545a <cpuid_get_feature_ECX>
  10453c:	89 44 24 28          	mov    DWORD PTR [esp+0x28],eax
    cid.RAW_EDX = cpuid_get_feature_EDX();
  104540:	e8 0b 0f 00 00       	call   105450 <cpuid_get_feature_EDX>
    
    return cid;
  104545:	8b 54 24 18          	mov    edx,DWORD PTR [esp+0x18]
  104549:	89 46 14             	mov    DWORD PTR [esi+0x14],eax
}
  10454c:	89 f0                	mov    eax,esi
    return cid;
  10454e:	89 16                	mov    DWORD PTR [esi],edx
  104550:	8b 54 24 1c          	mov    edx,DWORD PTR [esp+0x1c]
  104554:	89 56 04             	mov    DWORD PTR [esi+0x4],edx
  104557:	8b 54 24 20          	mov    edx,DWORD PTR [esp+0x20]
  10455b:	89 56 08             	mov    DWORD PTR [esi+0x8],edx
  10455e:	8b 54 24 24          	mov    edx,DWORD PTR [esp+0x24]
  104562:	89 56 0c             	mov    DWORD PTR [esi+0xc],edx
  104565:	8b 54 24 28          	mov    edx,DWORD PTR [esp+0x28]
  104569:	89 56 10             	mov    DWORD PTR [esi+0x10],edx
}
  10456c:	83 c4 34             	add    esp,0x34
  10456f:	5b                   	pop    ebx
  104570:	5e                   	pop    esi
  104571:	c2 04 00             	ret    0x4
  104574:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
  10457a:	8d bf 00 00 00 00    	lea    edi,[edi+0x0]

00104580 <cpuid_dump>:

void cpuid_dump()
{
  104580:	53                   	push   ebx
  104581:	83 ec 3c             	sub    esp,0x3c
    memset(&cid.vendorid, 0, 16);
  104584:	6a 10                	push   0x10
  104586:	6a 00                	push   0x0
  104588:	8d 44 24 24          	lea    eax,[esp+0x24]
  10458c:	50                   	push   eax
  10458d:	e8 3e 11 00 00       	call   1056d0 <memset>
    asm volatile ("cpuid":"=a"(*where),"=b"(*(where+0)),
  104592:	31 c0                	xor    eax,eax
  104594:	0f a2                	cpuid  
  104596:	89 54 24 2c          	mov    DWORD PTR [esp+0x2c],edx
  10459a:	89 4c 24 30          	mov    DWORD PTR [esp+0x30],ecx
  10459e:	89 5c 24 28          	mov    DWORD PTR [esp+0x28],ebx
    cid.RAW_ECX = cpuid_get_feature_ECX();
  1045a2:	e8 b3 0e 00 00       	call   10545a <cpuid_get_feature_ECX>
  1045a7:	89 44 24 38          	mov    DWORD PTR [esp+0x38],eax
    cid.RAW_EDX = cpuid_get_feature_EDX();
  1045ab:	e8 a0 0e 00 00       	call   105450 <cpuid_get_feature_EDX>
    return cid;
  1045b0:	8b 54 24 28          	mov    edx,DWORD PTR [esp+0x28]
    cid.RAW_EDX = cpuid_get_feature_EDX();
  1045b4:	89 44 24 3c          	mov    DWORD PTR [esp+0x3c],eax
    return cid;
  1045b8:	89 44 24 24          	mov    DWORD PTR [esp+0x24],eax
  1045bc:	89 54 24 10          	mov    DWORD PTR [esp+0x10],edx
  1045c0:	8b 54 24 2c          	mov    edx,DWORD PTR [esp+0x2c]
  1045c4:	89 54 24 14          	mov    DWORD PTR [esp+0x14],edx
  1045c8:	8b 54 24 30          	mov    edx,DWORD PTR [esp+0x30]
  1045cc:	89 54 24 18          	mov    DWORD PTR [esp+0x18],edx
  1045d0:	8b 54 24 34          	mov    edx,DWORD PTR [esp+0x34]
  1045d4:	89 54 24 1c          	mov    DWORD PTR [esp+0x1c],edx
  1045d8:	8b 54 24 38          	mov    edx,DWORD PTR [esp+0x38]
  1045dc:	89 54 24 20          	mov    DWORD PTR [esp+0x20],edx
    cpuid_t cid = cpuid();
    
    printf("&fCPUID dump:&7\n\tVendorid: %s\n\tFeatures:", cid.vendorid);    
  1045e0:	58                   	pop    eax
  1045e1:	5a                   	pop    edx
  1045e2:	8d 44 24 08          	lea    eax,[esp+0x8]
  1045e6:	50                   	push   eax
  1045e7:	68 c0 67 11 00       	push   0x1167c0
  1045ec:	e8 7f 20 00 00       	call   106670 <printf>
    // if (cid.POPCNT ) { puts(" POPCNT"); }  
    // if (cid.AES    ) { puts(" AES"); }  
    // if (cid.XSAVE  ) { puts(" XSAVE"); }  
    // if (cid.OSXSAVE) { puts(" OSXSAVE"); }  
    // if (cid.AVX    ) { puts(" AVX"); }  
    if (cid.FPU    ) { puts(" FPU"); }   
  1045f1:	0f b6 44 24 24       	movzx  eax,BYTE PTR [esp+0x24]
  1045f6:	83 c4 10             	add    esp,0x10
  1045f9:	a8 01                	test   al,0x1
  1045fb:	0f 85 87 00 00 00    	jne    104688 <cpuid_dump+0x108>
    // if (cid.VME    ) { puts(" VME"); }   
    // if (cid.DE     ) { puts(" DE"); }   
    // if (cid.PSE    ) { puts(" PSE"); }   
    // if (cid.TSC    ) { puts(" TSC"); }   
    // if (cid.MSR    ) { puts(" MSR"); }   
    if (cid.PAE    ) { puts(" PAE"); }   
  104601:	a8 40                	test   al,0x40
  104603:	75 53                	jne    104658 <cpuid_dump+0xd8>
    // if (cid.CLF    ) { puts(" CLF"); }   
    // if (cid.DTES   ) { puts(" DTES"); }   
    // if (cid.ACPI   ) { puts(" ACPI"); }   
    // if (cid.MMX    ) { puts(" MMX"); }   
    // if (cid.FXSR   ) { puts(" FXSR"); }   
    if (cid.SSE    ) { puts(" SSE"); }   
  104605:	80 7c 24 16 00       	cmp    BYTE PTR [esp+0x16],0x0
  10460a:	78 63                	js     10466f <cpuid_dump+0xef>
    if (cid.SSE2   ) { puts(" SSE2"); }   
  10460c:	f6 44 24 17 01       	test   BYTE PTR [esp+0x17],0x1
  104611:	75 2d                	jne    104640 <cpuid_dump+0xc0>
    if (cid.SSE3   ) { puts(" SSE3"); } 
  104613:	f6 44 24 10 01       	test   BYTE PTR [esp+0x10],0x1
  104618:	74 10                	je     10462a <cpuid_dump+0xaa>
  10461a:	83 ec 0c             	sub    esp,0xc
  10461d:	68 13 23 11 00       	push   0x112313
  104622:	e8 79 32 00 00       	call   1078a0 <puts>
  104627:	83 c4 10             	add    esp,0x10
    // if (cid.SS     ) { puts(" SS"); }   
    // if (cid.HTT    ) { puts(" HTT"); }   
    // if (cid.TM1    ) { puts(" TM1"); }   
    // if (cid.IA64   ) { puts(" IA64"); }   
    // if (cid.PBE    ) { puts(" PBE"); }
    puts("&7\n");
  10462a:	83 ec 0c             	sub    esp,0xc
  10462d:	68 19 23 11 00       	push   0x112319
  104632:	e8 69 32 00 00       	call   1078a0 <puts>
  104637:	83 c4 48             	add    esp,0x48
  10463a:	5b                   	pop    ebx
  10463b:	c3                   	ret    
  10463c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
    if (cid.SSE2   ) { puts(" SSE2"); }   
  104640:	83 ec 0c             	sub    esp,0xc
  104643:	68 0d 23 11 00       	push   0x11230d
  104648:	e8 53 32 00 00       	call   1078a0 <puts>
  10464d:	83 c4 10             	add    esp,0x10
  104650:	eb c1                	jmp    104613 <cpuid_dump+0x93>
  104652:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
    if (cid.PAE    ) { puts(" PAE"); }   
  104658:	83 ec 0c             	sub    esp,0xc
  10465b:	68 03 23 11 00       	push   0x112303
  104660:	e8 3b 32 00 00       	call   1078a0 <puts>
  104665:	83 c4 10             	add    esp,0x10
    if (cid.SSE    ) { puts(" SSE"); }   
  104668:	80 7c 24 16 00       	cmp    BYTE PTR [esp+0x16],0x0
  10466d:	79 9d                	jns    10460c <cpuid_dump+0x8c>
  10466f:	83 ec 0c             	sub    esp,0xc
  104672:	68 08 23 11 00       	push   0x112308
  104677:	e8 24 32 00 00       	call   1078a0 <puts>
  10467c:	83 c4 10             	add    esp,0x10
  10467f:	eb 8b                	jmp    10460c <cpuid_dump+0x8c>
  104681:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
    if (cid.FPU    ) { puts(" FPU"); }   
  104688:	83 ec 0c             	sub    esp,0xc
  10468b:	68 fe 22 11 00       	push   0x1122fe
  104690:	e8 0b 32 00 00       	call   1078a0 <puts>
  104695:	0f b6 44 24 24       	movzx  eax,BYTE PTR [esp+0x24]
  10469a:	83 c4 10             	add    esp,0x10
  10469d:	e9 5f ff ff ff       	jmp    104601 <cpuid_dump+0x81>
  1046a2:	66 90                	xchg   ax,ax
  1046a4:	66 90                	xchg   ax,ax
  1046a6:	66 90                	xchg   ax,ax
  1046a8:	66 90                	xchg   ax,ax
  1046aa:	66 90                	xchg   ax,ax
  1046ac:	66 90                	xchg   ax,ax
  1046ae:	66 90                	xchg   ax,ax

001046b0 <gdt_setup>:
gdt_t gdt;

extern void gdt_flush(u32);

void gdt_setup()
{
  1046b0:	53                   	push   ebx
    entry->limit16_19 = (limit >> 16) & 0x0f;
}

void gdt_tss_entry(int index, u16 ss0, u32 esp0)
{
    gdt_entry(index, (u32)&gdt.tss, sizeof(tss_t), PRESENT | EXECUTABLE | ACCESSED, TSS_FLAGS, "TSS");
  1046b1:	bb f6 a0 23 00       	mov    ebx,0x23a0f6
{
  1046b6:	83 ec 0c             	sub    esp,0xc
    log("GDT[%d:%s]\t = { BASE=%x, LIMIT=%x, ACCESS=%b, FLAGS=%b }", index, hint, base, limit, access, flags);
  1046b9:	6a 00                	push   0x0
  1046bb:	6a 00                	push   0x0
  1046bd:	6a 00                	push   0x0
  1046bf:	6a 00                	push   0x0
  1046c1:	68 1d 23 11 00       	push   0x11231d
  1046c6:	6a 00                	push   0x0
  1046c8:	68 ec 67 11 00       	push   0x1167ec
  1046cd:	e8 ce cf ff ff       	call   1016a0 <log>
  1046d2:	83 c4 1c             	add    esp,0x1c
    entry->limit0_15 = limit & 0xffff;
  1046d5:	c7 05 c0 a0 23 00 00 	mov    DWORD PTR ds:0x23a0c0,0x0
  1046dc:	00 00 00 
    entry->base0_15 = base & 0xffff;
  1046df:	c7 05 c4 a0 23 00 00 	mov    DWORD PTR ds:0x23a0c4,0x0
  1046e6:	00 00 00 
    log("GDT[%d:%s]\t = { BASE=%x, LIMIT=%x, ACCESS=%b, FLAGS=%b }", index, hint, base, limit, access, flags);
  1046e9:	6a 0c                	push   0xc
  1046eb:	68 9a 00 00 00       	push   0x9a
  1046f0:	6a ff                	push   0xffffffff
  1046f2:	6a 00                	push   0x0
  1046f4:	68 22 23 11 00       	push   0x112322
  1046f9:	6a 01                	push   0x1
  1046fb:	68 ec 67 11 00       	push   0x1167ec
  104700:	e8 9b cf ff ff       	call   1016a0 <log>
  104705:	83 c4 1c             	add    esp,0x1c
    entry->limit0_15 = limit & 0xffff;
  104708:	c7 05 c8 a0 23 00 ff 	mov    DWORD PTR ds:0x23a0c8,0xffff
  10470f:	ff 00 00 
    entry->base0_15 = base & 0xffff;
  104712:	c7 05 cc a0 23 00 00 	mov    DWORD PTR ds:0x23a0cc,0xcf9a00
  104719:	9a cf 00 
    log("GDT[%d:%s]\t = { BASE=%x, LIMIT=%x, ACCESS=%b, FLAGS=%b }", index, hint, base, limit, access, flags);
  10471c:	6a 0c                	push   0xc
  10471e:	68 92 00 00 00       	push   0x92
  104723:	6a ff                	push   0xffffffff
  104725:	6a 00                	push   0x0
  104727:	68 28 23 11 00       	push   0x112328
  10472c:	6a 02                	push   0x2
  10472e:	68 ec 67 11 00       	push   0x1167ec
  104733:	e8 68 cf ff ff       	call   1016a0 <log>
  104738:	83 c4 1c             	add    esp,0x1c
    entry->limit0_15 = limit & 0xffff;
  10473b:	c7 05 d0 a0 23 00 ff 	mov    DWORD PTR ds:0x23a0d0,0xffff
  104742:	ff 00 00 
    entry->base0_15 = base & 0xffff;
  104745:	c7 05 d4 a0 23 00 00 	mov    DWORD PTR ds:0x23a0d4,0xcf9200
  10474c:	92 cf 00 
    log("GDT[%d:%s]\t = { BASE=%x, LIMIT=%x, ACCESS=%b, FLAGS=%b }", index, hint, base, limit, access, flags);
  10474f:	6a 0c                	push   0xc
  104751:	68 fa 00 00 00       	push   0xfa
  104756:	6a ff                	push   0xffffffff
  104758:	6a 00                	push   0x0
  10475a:	68 2e 23 11 00       	push   0x11232e
  10475f:	6a 03                	push   0x3
  104761:	68 ec 67 11 00       	push   0x1167ec
  104766:	e8 35 cf ff ff       	call   1016a0 <log>
  10476b:	83 c4 1c             	add    esp,0x1c
    entry->limit0_15 = limit & 0xffff;
  10476e:	c7 05 d8 a0 23 00 ff 	mov    DWORD PTR ds:0x23a0d8,0xffff
  104775:	ff 00 00 
    entry->base0_15 = base & 0xffff;
  104778:	c7 05 dc a0 23 00 00 	mov    DWORD PTR ds:0x23a0dc,0xcffa00
  10477f:	fa cf 00 
    log("GDT[%d:%s]\t = { BASE=%x, LIMIT=%x, ACCESS=%b, FLAGS=%b }", index, hint, base, limit, access, flags);
  104782:	6a 0c                	push   0xc
  104784:	68 f2 00 00 00       	push   0xf2
  104789:	6a ff                	push   0xffffffff
  10478b:	6a 00                	push   0x0
  10478d:	68 34 23 11 00       	push   0x112334
  104792:	6a 04                	push   0x4
  104794:	68 ec 67 11 00       	push   0x1167ec
  104799:	e8 02 cf ff ff       	call   1016a0 <log>
  10479e:	83 c4 1c             	add    esp,0x1c
    entry->limit0_15 = limit & 0xffff;
  1047a1:	c7 05 e0 a0 23 00 ff 	mov    DWORD PTR ds:0x23a0e0,0xffff
  1047a8:	ff 00 00 
    entry->base0_15 = base & 0xffff;
  1047ab:	c7 05 e4 a0 23 00 00 	mov    DWORD PTR ds:0x23a0e4,0xcff200
  1047b2:	f2 cf 00 
    log("GDT[%d:%s]\t = { BASE=%x, LIMIT=%x, ACCESS=%b, FLAGS=%b }", index, hint, base, limit, access, flags);
  1047b5:	6a 00                	push   0x0
  1047b7:	68 99 00 00 00       	push   0x99
  1047bc:	6a 68                	push   0x68
  1047be:	53                   	push   ebx
  1047bf:	68 de 23 11 00       	push   0x1123de
  1047c4:	6a 05                	push   0x5
  1047c6:	68 ec 67 11 00       	push   0x1167ec
  1047cb:	e8 d0 ce ff ff       	call   1016a0 <log>
    memset(&gdt.tss, 0, sizeof(tss_t));
  1047d0:	83 c4 1c             	add    esp,0x1c
    entry->base16_23 = (base >> 16) & 0xff;
  1047d3:	89 d8                	mov    eax,ebx
    entry->acces = access;
  1047d5:	ba 99 00 00 00       	mov    edx,0x99
    memset(&gdt.tss, 0, sizeof(tss_t));
  1047da:	6a 68                	push   0x68
    entry->base16_23 = (base >> 16) & 0xff;
  1047dc:	c1 e8 10             	shr    eax,0x10
    memset(&gdt.tss, 0, sizeof(tss_t));
  1047df:	6a 00                	push   0x0
  1047e1:	68 f6 a0 23 00       	push   0x23a0f6
    entry->base0_15 = base & 0xffff;
  1047e6:	66 89 1d ea a0 23 00 	mov    WORD PTR ds:0x23a0ea,bx
    entry->base24_31 = (base >> 24) & 0xff;
  1047ed:	c1 eb 18             	shr    ebx,0x18
    entry->base16_23 = (base >> 16) & 0xff;
  1047f0:	a2 ec a0 23 00       	mov    ds:0x23a0ec,al
    entry->limit0_15 = limit & 0xffff;
  1047f5:	b8 68 00 00 00       	mov    eax,0x68
    entry->base24_31 = (base >> 24) & 0xff;
  1047fa:	88 1d ef a0 23 00    	mov    BYTE PTR ds:0x23a0ef,bl
    entry->acces = access;
  104800:	66 89 15 ed a0 23 00 	mov    WORD PTR ds:0x23a0ed,dx
    entry->limit0_15 = limit & 0xffff;
  104807:	66 a3 e8 a0 23 00    	mov    ds:0x23a0e8,ax
    memset(&gdt.tss, 0, sizeof(tss_t));
  10480d:	e8 be 0e 00 00       	call   1056d0 <memset>
    gdt.descriptor.size = (sizeof(gdt_entry_t) * GDT_ENTRY_COUNT) - 1;
  104812:	b9 2f 00 00 00       	mov    ecx,0x2f
    gdt_flush((u32)&gdt.descriptor);
  104817:	c7 04 24 f0 a0 23 00 	mov    DWORD PTR [esp],0x23a0f0

    tss_t* tss = &gdt.tss;

    tss->ss0 = ss0;
  10481e:	c7 05 fe a0 23 00 10 	mov    DWORD PTR ds:0x23a0fe,0x10
  104825:	00 00 00 
    tss->esp0 = esp0;
  104828:	c7 05 fa a0 23 00 00 	mov    DWORD PTR ds:0x23a0fa,0x0
  10482f:	00 00 00 

    tss->cs = 0x0b;
  104832:	c7 05 42 a1 23 00 0b 	mov    DWORD PTR ds:0x23a142,0xb
  104839:	00 00 00 
	tss->ss = 0x13;
  10483c:	c7 05 46 a1 23 00 13 	mov    DWORD PTR ds:0x23a146,0x13
  104843:	00 00 00 
	tss->ds = 0x13;
  104846:	c7 05 4a a1 23 00 13 	mov    DWORD PTR ds:0x23a14a,0x13
  10484d:	00 00 00 
	tss->es = 0x13;
  104850:	c7 05 3e a1 23 00 13 	mov    DWORD PTR ds:0x23a13e,0x13
  104857:	00 00 00 
	tss->fs = 0x13;
  10485a:	c7 05 4e a1 23 00 13 	mov    DWORD PTR ds:0x23a14e,0x13
  104861:	00 00 00 
	tss->gs = 0x13;
  104864:	c7 05 52 a1 23 00 13 	mov    DWORD PTR ds:0x23a152,0x13
  10486b:	00 00 00 
    gdt.descriptor.offset = (u32)&gdt.entries[0];
  10486e:	c7 05 f2 a0 23 00 c0 	mov    DWORD PTR ds:0x23a0f2,0x23a0c0
  104875:	a0 23 00 
    gdt.descriptor.size = (sizeof(gdt_entry_t) * GDT_ENTRY_COUNT) - 1;
  104878:	66 89 0d f0 a0 23 00 	mov    WORD PTR ds:0x23a0f0,cx
    gdt_flush((u32)&gdt.descriptor);
  10487f:	e8 a7 0b 00 00       	call   10542b <gdt_flush>
}
  104884:	83 c4 18             	add    esp,0x18
  104887:	5b                   	pop    ebx
  104888:	c3                   	ret    
  104889:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]

00104890 <set_kernel_stack>:
    gdt.tss.esp0 = stack;
  104890:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  104894:	a3 fa a0 23 00       	mov    ds:0x23a0fa,eax
}
  104899:	c3                   	ret    
  10489a:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]

001048a0 <gdt_entry>:
{
  1048a0:	55                   	push   ebp
  1048a1:	57                   	push   edi
  1048a2:	56                   	push   esi
  1048a3:	53                   	push   ebx
  1048a4:	83 ec 20             	sub    esp,0x20
  1048a7:	8b 7c 24 44          	mov    edi,DWORD PTR [esp+0x44]
  1048ab:	8b 44 24 40          	mov    eax,DWORD PTR [esp+0x40]
  1048af:	8b 5c 24 34          	mov    ebx,DWORD PTR [esp+0x34]
  1048b3:	8b 6c 24 38          	mov    ebp,DWORD PTR [esp+0x38]
  1048b7:	8b 74 24 3c          	mov    esi,DWORD PTR [esp+0x3c]
    log("GDT[%d:%s]\t = { BASE=%x, LIMIT=%x, ACCESS=%b, FLAGS=%b }", index, hint, base, limit, access, flags);
  1048bb:	89 f9                	mov    ecx,edi
    entry->limit16_19 = (limit >> 16) & 0x0f;
  1048bd:	c1 e7 04             	shl    edi,0x4
    log("GDT[%d:%s]\t = { BASE=%x, LIMIT=%x, ACCESS=%b, FLAGS=%b }", index, hint, base, limit, access, flags);
  1048c0:	0f b6 d1             	movzx  edx,cl
  1048c3:	52                   	push   edx
  1048c4:	0f b6 d0             	movzx  edx,al
  1048c7:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  1048cb:	52                   	push   edx
  1048cc:	56                   	push   esi
  1048cd:	55                   	push   ebp
  1048ce:	ff 74 24 58          	push   DWORD PTR [esp+0x58]
  1048d2:	53                   	push   ebx
  1048d3:	68 ec 67 11 00       	push   0x1167ec
  1048d8:	e8 c3 cd ff ff       	call   1016a0 <log>
    entry->acces = access;
  1048dd:	8b 44 24 2c          	mov    eax,DWORD PTR [esp+0x2c]
    entry->limit0_15 = limit & 0xffff;
  1048e1:	66 89 34 dd c0 a0 23 	mov    WORD PTR [ebx*8+0x23a0c0],si
  1048e8:	00 
    entry->limit16_19 = (limit >> 16) & 0x0f;
  1048e9:	c1 ee 10             	shr    esi,0x10
  1048ec:	83 e6 0f             	and    esi,0xf
    entry->base0_15 = base & 0xffff;
  1048ef:	66 89 2c dd c2 a0 23 	mov    WORD PTR [ebx*8+0x23a0c2],bp
  1048f6:	00 
    entry->limit16_19 = (limit >> 16) & 0x0f;
  1048f7:	09 fe                	or     esi,edi
    entry->acces = access;
  1048f9:	88 04 dd c5 a0 23 00 	mov    BYTE PTR [ebx*8+0x23a0c5],al
    entry->base16_23 = (base >> 16) & 0xff;
  104900:	89 e8                	mov    eax,ebp
  104902:	c1 e8 10             	shr    eax,0x10
  104905:	88 04 dd c4 a0 23 00 	mov    BYTE PTR [ebx*8+0x23a0c4],al
    entry->base24_31 = (base >> 24) & 0xff;
  10490c:	89 e8                	mov    eax,ebp
  10490e:	c1 e8 18             	shr    eax,0x18
  104911:	88 04 dd c7 a0 23 00 	mov    BYTE PTR [ebx*8+0x23a0c7],al
    entry->limit16_19 = (limit >> 16) & 0x0f;
  104918:	89 f0                	mov    eax,esi
  10491a:	88 04 dd c6 a0 23 00 	mov    BYTE PTR [ebx*8+0x23a0c6],al
}
  104921:	83 c4 3c             	add    esp,0x3c
  104924:	5b                   	pop    ebx
  104925:	5e                   	pop    esi
  104926:	5f                   	pop    edi
  104927:	5d                   	pop    ebp
  104928:	c3                   	ret    
  104929:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]

00104930 <gdt_tss_entry>:
{
  104930:	57                   	push   edi
  104931:	56                   	push   esi
    gdt_entry(index, (u32)&gdt.tss, sizeof(tss_t), PRESENT | EXECUTABLE | ACCESSED, TSS_FLAGS, "TSS");
  104932:	be f6 a0 23 00       	mov    esi,0x23a0f6
{
  104937:	53                   	push   ebx
  104938:	8b 5c 24 10          	mov    ebx,DWORD PTR [esp+0x10]
  10493c:	8b 7c 24 14          	mov    edi,DWORD PTR [esp+0x14]
    log("GDT[%d:%s]\t = { BASE=%x, LIMIT=%x, ACCESS=%b, FLAGS=%b }", index, hint, base, limit, access, flags);
  104940:	83 ec 04             	sub    esp,0x4
  104943:	6a 00                	push   0x0
  104945:	68 99 00 00 00       	push   0x99
  10494a:	6a 68                	push   0x68
  10494c:	56                   	push   esi
    tss->ss0 = ss0;
  10494d:	0f b7 ff             	movzx  edi,di
    log("GDT[%d:%s]\t = { BASE=%x, LIMIT=%x, ACCESS=%b, FLAGS=%b }", index, hint, base, limit, access, flags);
  104950:	68 de 23 11 00       	push   0x1123de
  104955:	53                   	push   ebx
  104956:	68 ec 67 11 00       	push   0x1167ec
  10495b:	e8 40 cd ff ff       	call   1016a0 <log>
    entry->base16_23 = (base >> 16) & 0xff;
  104960:	89 f0                	mov    eax,esi
    memset(&gdt.tss, 0, sizeof(tss_t));
  104962:	83 c4 1c             	add    esp,0x1c
    entry->base0_15 = base & 0xffff;
  104965:	66 89 34 dd c2 a0 23 	mov    WORD PTR [ebx*8+0x23a0c2],si
  10496c:	00 
    entry->base16_23 = (base >> 16) & 0xff;
  10496d:	c1 e8 10             	shr    eax,0x10
    memset(&gdt.tss, 0, sizeof(tss_t));
  104970:	6a 68                	push   0x68
  104972:	6a 00                	push   0x0
    entry->base16_23 = (base >> 16) & 0xff;
  104974:	88 04 dd c4 a0 23 00 	mov    BYTE PTR [ebx*8+0x23a0c4],al
    entry->base24_31 = (base >> 24) & 0xff;
  10497b:	89 f0                	mov    eax,esi
    memset(&gdt.tss, 0, sizeof(tss_t));
  10497d:	56                   	push   esi
    entry->base24_31 = (base >> 24) & 0xff;
  10497e:	c1 e8 18             	shr    eax,0x18
    entry->acces = access;
  104981:	c6 04 dd c5 a0 23 00 	mov    BYTE PTR [ebx*8+0x23a0c5],0x99
  104988:	99 
    entry->limit16_19 = (limit >> 16) & 0x0f;
  104989:	c6 04 dd c6 a0 23 00 	mov    BYTE PTR [ebx*8+0x23a0c6],0x0
  104990:	00 
    entry->base24_31 = (base >> 24) & 0xff;
  104991:	88 04 dd c7 a0 23 00 	mov    BYTE PTR [ebx*8+0x23a0c7],al
    entry->limit0_15 = limit & 0xffff;
  104998:	b8 68 00 00 00       	mov    eax,0x68
  10499d:	66 89 04 dd c0 a0 23 	mov    WORD PTR [ebx*8+0x23a0c0],ax
  1049a4:	00 
    memset(&gdt.tss, 0, sizeof(tss_t));
  1049a5:	e8 26 0d 00 00       	call   1056d0 <memset>
    tss->esp0 = esp0;
  1049aa:	8b 44 24 28          	mov    eax,DWORD PTR [esp+0x28]
}
  1049ae:	83 c4 10             	add    esp,0x10
    tss->ss0 = ss0;
  1049b1:	89 3d fe a0 23 00    	mov    DWORD PTR ds:0x23a0fe,edi
}
  1049b7:	5b                   	pop    ebx
    tss->cs = 0x0b;
  1049b8:	c7 05 42 a1 23 00 0b 	mov    DWORD PTR ds:0x23a142,0xb
  1049bf:	00 00 00 
	tss->ss = 0x13;
  1049c2:	c7 05 46 a1 23 00 13 	mov    DWORD PTR ds:0x23a146,0x13
  1049c9:	00 00 00 
}
  1049cc:	5e                   	pop    esi
    tss->esp0 = esp0;
  1049cd:	a3 fa a0 23 00       	mov    ds:0x23a0fa,eax
	tss->ds = 0x13;
  1049d2:	c7 05 4a a1 23 00 13 	mov    DWORD PTR ds:0x23a14a,0x13
  1049d9:	00 00 00 
	tss->es = 0x13;
  1049dc:	c7 05 3e a1 23 00 13 	mov    DWORD PTR ds:0x23a13e,0x13
  1049e3:	00 00 00 
	tss->fs = 0x13;
  1049e6:	c7 05 4e a1 23 00 13 	mov    DWORD PTR ds:0x23a14e,0x13
  1049ed:	00 00 00 
	tss->gs = 0x13;
  1049f0:	c7 05 52 a1 23 00 13 	mov    DWORD PTR ds:0x23a152,0x13
  1049f7:	00 00 00 
}
  1049fa:	5f                   	pop    edi
  1049fb:	c3                   	ret    
  1049fc:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]

00104a00 <gdt_dump>:

/* --- debug ---------------------------------------------------------------- */

void gdt_dump()
{
  104a00:	53                   	push   ebx
    puts("\nGlobal descriptor table dump:\n");
    for(u32 i = 0; i < GDT_ENTRY_COUNT; i++)
  104a01:	31 db                	xor    ebx,ebx
{
  104a03:	83 ec 14             	sub    esp,0x14
    puts("\nGlobal descriptor table dump:\n");
  104a06:	68 28 68 11 00       	push   0x116828
  104a0b:	e8 90 2e 00 00       	call   1078a0 <puts>
  104a10:	83 c4 10             	add    esp,0x10
  104a13:	90                   	nop
  104a14:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
    {
        puts("\t");
  104a18:	83 ec 0c             	sub    esp,0xc
  104a1b:	68 d6 1f 11 00       	push   0x111fd6
  104a20:	e8 7b 2e 00 00       	call   1078a0 <puts>
}

void gdt_entry_dump(int index)
{
    gdt_entry_t * entry = &gdt.entries[index];
    printf("%d: base=%x limite=%x access=%x flags=%x\n", 
  104a25:	58                   	pop    eax
        index,
        entry->base0_15 | entry->base16_23 << 16 | entry->base24_31 << 24,
        entry->limit0_15 | entry->limit16_19 << 16,
        entry->acces, entry->flags);
  104a26:	0f b6 04 dd c6 a0 23 	movzx  eax,BYTE PTR [ebx*8+0x23a0c6]
  104a2d:	00 
    printf("%d: base=%x limite=%x access=%x flags=%x\n", 
  104a2e:	5a                   	pop    edx
        entry->acces, entry->flags);
  104a2f:	89 c2                	mov    edx,eax
        entry->limit0_15 | entry->limit16_19 << 16,
  104a31:	83 e0 0f             	and    eax,0xf
        entry->acces, entry->flags);
  104a34:	c0 ea 04             	shr    dl,0x4
        entry->limit0_15 | entry->limit16_19 << 16,
  104a37:	c1 e0 10             	shl    eax,0x10
    printf("%d: base=%x limite=%x access=%x flags=%x\n", 
  104a3a:	0f b6 d2             	movzx  edx,dl
  104a3d:	52                   	push   edx
  104a3e:	0f b6 14 dd c5 a0 23 	movzx  edx,BYTE PTR [ebx*8+0x23a0c5]
  104a45:	00 
  104a46:	52                   	push   edx
        entry->limit0_15 | entry->limit16_19 << 16,
  104a47:	0f b7 14 dd c0 a0 23 	movzx  edx,WORD PTR [ebx*8+0x23a0c0]
  104a4e:	00 
    printf("%d: base=%x limite=%x access=%x flags=%x\n", 
  104a4f:	09 d0                	or     eax,edx
        entry->base0_15 | entry->base16_23 << 16 | entry->base24_31 << 24,
  104a51:	0f b6 14 dd c7 a0 23 	movzx  edx,BYTE PTR [ebx*8+0x23a0c7]
  104a58:	00 
    printf("%d: base=%x limite=%x access=%x flags=%x\n", 
  104a59:	50                   	push   eax
        entry->base0_15 | entry->base16_23 << 16 | entry->base24_31 << 24,
  104a5a:	0f b6 04 dd c4 a0 23 	movzx  eax,BYTE PTR [ebx*8+0x23a0c4]
  104a61:	00 
  104a62:	c1 e2 18             	shl    edx,0x18
  104a65:	c1 e0 10             	shl    eax,0x10
    printf("%d: base=%x limite=%x access=%x flags=%x\n", 
  104a68:	09 d0                	or     eax,edx
        entry->base0_15 | entry->base16_23 << 16 | entry->base24_31 << 24,
  104a6a:	0f b7 14 dd c2 a0 23 	movzx  edx,WORD PTR [ebx*8+0x23a0c2]
  104a71:	00 
    printf("%d: base=%x limite=%x access=%x flags=%x\n", 
  104a72:	09 d0                	or     eax,edx
  104a74:	50                   	push   eax
  104a75:	53                   	push   ebx
    for(u32 i = 0; i < GDT_ENTRY_COUNT; i++)
  104a76:	83 c3 01             	add    ebx,0x1
    printf("%d: base=%x limite=%x access=%x flags=%x\n", 
  104a79:	68 48 68 11 00       	push   0x116848
  104a7e:	e8 ed 1b 00 00       	call   106670 <printf>
    for(u32 i = 0; i < GDT_ENTRY_COUNT; i++)
  104a83:	83 c4 20             	add    esp,0x20
  104a86:	83 fb 06             	cmp    ebx,0x6
  104a89:	75 8d                	jne    104a18 <gdt_dump+0x18>
    puts("\n");
  104a8b:	83 ec 0c             	sub    esp,0xc
  104a8e:	68 f9 1f 11 00       	push   0x111ff9
  104a93:	e8 08 2e 00 00       	call   1078a0 <puts>
}
  104a98:	83 c4 18             	add    esp,0x18
  104a9b:	5b                   	pop    ebx
  104a9c:	c3                   	ret    
  104a9d:	8d 76 00             	lea    esi,[esi+0x0]

00104aa0 <gdt_entry_dump>:
{
  104aa0:	83 ec 14             	sub    esp,0x14
  104aa3:	8b 54 24 18          	mov    edx,DWORD PTR [esp+0x18]
        entry->acces, entry->flags);
  104aa7:	0f b6 04 d5 c6 a0 23 	movzx  eax,BYTE PTR [edx*8+0x23a0c6]
  104aae:	00 
  104aaf:	89 c1                	mov    ecx,eax
        entry->limit0_15 | entry->limit16_19 << 16,
  104ab1:	83 e0 0f             	and    eax,0xf
        entry->acces, entry->flags);
  104ab4:	c0 e9 04             	shr    cl,0x4
        entry->limit0_15 | entry->limit16_19 << 16,
  104ab7:	c1 e0 10             	shl    eax,0x10
    printf("%d: base=%x limite=%x access=%x flags=%x\n", 
  104aba:	0f b6 c9             	movzx  ecx,cl
  104abd:	51                   	push   ecx
  104abe:	0f b6 0c d5 c5 a0 23 	movzx  ecx,BYTE PTR [edx*8+0x23a0c5]
  104ac5:	00 
  104ac6:	51                   	push   ecx
        entry->limit0_15 | entry->limit16_19 << 16,
  104ac7:	0f b7 0c d5 c0 a0 23 	movzx  ecx,WORD PTR [edx*8+0x23a0c0]
  104ace:	00 
    printf("%d: base=%x limite=%x access=%x flags=%x\n", 
  104acf:	09 c8                	or     eax,ecx
        entry->base0_15 | entry->base16_23 << 16 | entry->base24_31 << 24,
  104ad1:	0f b6 0c d5 c7 a0 23 	movzx  ecx,BYTE PTR [edx*8+0x23a0c7]
  104ad8:	00 
    printf("%d: base=%x limite=%x access=%x flags=%x\n", 
  104ad9:	50                   	push   eax
        entry->base0_15 | entry->base16_23 << 16 | entry->base24_31 << 24,
  104ada:	0f b6 04 d5 c4 a0 23 	movzx  eax,BYTE PTR [edx*8+0x23a0c4]
  104ae1:	00 
  104ae2:	c1 e1 18             	shl    ecx,0x18
  104ae5:	c1 e0 10             	shl    eax,0x10
    printf("%d: base=%x limite=%x access=%x flags=%x\n", 
  104ae8:	09 c8                	or     eax,ecx
        entry->base0_15 | entry->base16_23 << 16 | entry->base24_31 << 24,
  104aea:	0f b7 0c d5 c2 a0 23 	movzx  ecx,WORD PTR [edx*8+0x23a0c2]
  104af1:	00 
    printf("%d: base=%x limite=%x access=%x flags=%x\n", 
  104af2:	09 c8                	or     eax,ecx
  104af4:	50                   	push   eax
  104af5:	52                   	push   edx
  104af6:	68 48 68 11 00       	push   0x116848
  104afb:	e8 70 1b 00 00       	call   106670 <printf>
  104b00:	83 c4 2c             	add    esp,0x2c
  104b03:	c3                   	ret    
  104b04:	66 90                	xchg   ax,ax
  104b06:	66 90                	xchg   ax,ax
  104b08:	66 90                	xchg   ax,ax
  104b0a:	66 90                	xchg   ax,ax
  104b0c:	66 90                	xchg   ax,ax
  104b0e:	66 90                	xchg   ax,ax

00104b10 <idt_setup>:
// define in interupts.S
extern u32 int_handlers[];
extern void load_idt(u32);

void idt_setup()
{
  104b10:	83 ec 0c             	sub    esp,0xc
    pic_setup();
  104b13:	e8 78 01 00 00       	call   104c90 <pic_setup>

    // cleanup the idt.
    memset(&idt, 0, sizeof(idt_t));
  104b18:	83 ec 04             	sub    esp,0x4
  104b1b:	68 06 0c 00 00       	push   0xc06
  104b20:	6a 00                	push   0x0
  104b22:	68 60 a1 23 00       	push   0x23a160
  104b27:	e8 a4 0b 00 00       	call   1056d0 <memset>

    // setup the descriptor.
    idt.descriptor.offset = (u32)&idt.entries;
    idt.descriptor.size = sizeof(idt_entry_t) * IDT_ENTRY_COUNT;
  104b2c:	b8 00 08 00 00       	mov    eax,0x800

    // load the idt
    load_idt((u32)&idt.descriptor);
  104b31:	c7 04 24 60 a9 23 00 	mov    DWORD PTR [esp],0x23a960
    idt.descriptor.offset = (u32)&idt.entries;
  104b38:	c7 05 62 a9 23 00 60 	mov    DWORD PTR ds:0x23a962,0x23a160
  104b3f:	a1 23 00 
    idt.descriptor.size = sizeof(idt_entry_t) * IDT_ENTRY_COUNT;
  104b42:	66 a3 60 a9 23 00    	mov    ds:0x23a960,ax
    load_idt((u32)&idt.descriptor);
  104b48:	e8 fb 08 00 00       	call   105448 <load_idt>
}
  104b4d:	83 c4 1c             	add    esp,0x1c
  104b50:	c3                   	ret    
  104b51:	eb 0d                	jmp    104b60 <idt_entry>
  104b53:	90                   	nop
  104b54:	90                   	nop
  104b55:	90                   	nop
  104b56:	90                   	nop
  104b57:	90                   	nop
  104b58:	90                   	nop
  104b59:	90                   	nop
  104b5a:	90                   	nop
  104b5b:	90                   	nop
  104b5c:	90                   	nop
  104b5d:	90                   	nop
  104b5e:	90                   	nop
  104b5f:	90                   	nop

00104b60 <idt_entry>:

void idt_entry(u8 index, u32 offset, u16 selector, u16 type)
{
    idt_entry_t * entry = &idt.entries[index];
  104b60:	0f b6 44 24 04       	movzx  eax,BYTE PTR [esp+0x4]
{
  104b65:	8b 54 24 08          	mov    edx,DWORD PTR [esp+0x8]

    entry->offset0_15 = offset & 0xffff;
  104b69:	66 89 14 c5 60 a1 23 	mov    WORD PTR [eax*8+0x23a160],dx
  104b70:	00 
    entry->offset16_31 = (offset>>16) & 0xffff;
  104b71:	c1 ea 10             	shr    edx,0x10

    entry->zero = 0;
  104b74:	c6 04 c5 64 a1 23 00 	mov    BYTE PTR [eax*8+0x23a164],0x0
  104b7b:	00 
    entry->offset16_31 = (offset>>16) & 0xffff;
  104b7c:	66 89 14 c5 66 a1 23 	mov    WORD PTR [eax*8+0x23a166],dx
  104b83:	00 

    entry->selector = selector;
  104b84:	8b 54 24 0c          	mov    edx,DWORD PTR [esp+0xc]
  104b88:	66 89 14 c5 62 a1 23 	mov    WORD PTR [eax*8+0x23a162],dx
  104b8f:	00 
    entry->type_attr = type | 0x60;
  104b90:	0f b6 54 24 10       	movzx  edx,BYTE PTR [esp+0x10]
  104b95:	83 ca 60             	or     edx,0x60
  104b98:	88 14 c5 65 a1 23 00 	mov    BYTE PTR [eax*8+0x23a165],dl
  104b9f:	c3                   	ret    

00104ba0 <isr_setup>:
extern u32 isr_vector[];
extern bool console_bypass_lock;
isr_handler_t isr_handlers[32];

void isr_setup()
{
  104ba0:	53                   	push   ebx
	for (u32 i = 0; i < 32; i++)
  104ba1:	31 db                	xor    ebx,ebx
{
  104ba3:	83 ec 08             	sub    esp,0x8
  104ba6:	8d 76 00             	lea    esi,[esi+0x0]
  104ba9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
	{
		idt_entry(i, isr_vector[i], 0x08, INTGATE);
  104bb0:	68 8e 00 00 00       	push   0x8e
  104bb5:	6a 08                	push   0x8
  104bb7:	ff 34 9d 87 53 10 00 	push   DWORD PTR [ebx*4+0x105387]
  104bbe:	53                   	push   ebx
	for (u32 i = 0; i < 32; i++)
  104bbf:	83 c3 01             	add    ebx,0x1
		idt_entry(i, isr_vector[i], 0x08, INTGATE);
  104bc2:	e8 99 ff ff ff       	call   104b60 <idt_entry>
	for (u32 i = 0; i < 32; i++)
  104bc7:	83 c4 10             	add    esp,0x10
  104bca:	83 fb 20             	cmp    ebx,0x20
  104bcd:	75 e1                	jne    104bb0 <isr_setup+0x10>
	}

	// syscall handler
	idt_entry(128, isr_vector[32], 0x08, TRAPGATE);
  104bcf:	68 ef 00 00 00       	push   0xef
  104bd4:	6a 08                	push   0x8
  104bd6:	ff 35 07 54 10 00    	push   DWORD PTR ds:0x105407
  104bdc:	68 80 00 00 00       	push   0x80
  104be1:	e8 7a ff ff ff       	call   104b60 <idt_entry>
}
  104be6:	83 c4 18             	add    esp,0x18
  104be9:	5b                   	pop    ebx
  104bea:	c3                   	ret    
  104beb:	90                   	nop
  104bec:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]

00104bf0 <isr_register>:

isr_handler_t isr_register(int index, isr_handler_t handler)
{
  104bf0:	8b 54 24 04          	mov    edx,DWORD PTR [esp+0x4]
  104bf4:	8b 44 24 08          	mov    eax,DWORD PTR [esp+0x8]
	if (index < 32)
  104bf8:	83 fa 1f             	cmp    edx,0x1f
  104bfb:	7f 0b                	jg     104c08 <isr_register+0x18>
	{
		isr_handlers[index] = handler;
  104bfd:	89 04 95 80 ad 23 00 	mov    DWORD PTR [edx*4+0x23ad80],eax
		return handler;
  104c04:	c3                   	ret    
  104c05:	8d 76 00             	lea    esi,[esi+0x0]
	}

	return NULL;
  104c08:	31 c0                	xor    eax,eax
}
  104c0a:	c3                   	ret    
  104c0b:	90                   	nop
  104c0c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]

00104c10 <isr_handler>:

void isr_handler(context_t context)
{
  104c10:	83 ec 0c             	sub    esp,0xc

	if (isr_handlers[context.int_no] != NULL)
  104c13:	8b 54 24 40          	mov    edx,DWORD PTR [esp+0x40]
  104c17:	8b 04 95 80 ad 23 00 	mov    eax,DWORD PTR [edx*4+0x23ad80]
  104c1e:	85 c0                	test   eax,eax
  104c20:	74 1e                	je     104c40 <isr_handler+0x30>
	{
		isr_handlers[context.int_no](&context);
  104c22:	83 ec 0c             	sub    esp,0xc
  104c25:	8d 54 24 1c          	lea    edx,[esp+0x1c]
  104c29:	52                   	push   edx
  104c2a:	ff d0                	call   eax
  104c2c:	83 c4 10             	add    esp,0x10
		{
			CPANIC(&context, "CPU EXCEPTION: '%s' (INT:%d ERR:%x) !", exception_messages[context.int_no], context.int_no, context.errcode);
		}
	}

	outb(0x20, 0x20);
  104c2f:	83 ec 08             	sub    esp,0x8
  104c32:	6a 20                	push   0x20
  104c34:	6a 20                	push   0x20
  104c36:	e8 d5 f7 ff ff       	call   104410 <outb>
  104c3b:	83 c4 1c             	add    esp,0x1c
  104c3e:	c3                   	ret    
  104c3f:	90                   	nop
		if (context.int_no == 128)
  104c40:	81 fa 80 00 00 00    	cmp    edx,0x80
  104c46:	74 30                	je     104c78 <isr_handler+0x68>
			CPANIC(&context, "CPU EXCEPTION: '%s' (INT:%d ERR:%x) !", exception_messages[context.int_no], context.int_no, context.errcode);
  104c48:	ff 74 24 44          	push   DWORD PTR [esp+0x44]
  104c4c:	52                   	push   edx
  104c4d:	ff 34 95 00 1e 11 00 	push   DWORD PTR [edx*4+0x111e00]
  104c54:	68 74 68 11 00       	push   0x116874
  104c59:	8d 44 24 20          	lea    eax,[esp+0x20]
  104c5d:	50                   	push   eax
  104c5e:	6a 56                	push   0x56
  104c60:	68 e0 1d 11 00       	push   0x111de0
  104c65:	68 9c 68 11 00       	push   0x11689c
  104c6a:	e8 41 d4 ff ff       	call   1020b0 <__panic>
  104c6f:	83 c4 20             	add    esp,0x20
  104c72:	eb bb                	jmp    104c2f <isr_handler+0x1f>
  104c74:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
			syscall_dispatcher(&context);
  104c78:	83 ec 0c             	sub    esp,0xc
  104c7b:	8d 44 24 1c          	lea    eax,[esp+0x1c]
  104c7f:	50                   	push   eax
  104c80:	e8 7b e9 ff ff       	call   103600 <syscall_dispatcher>
  104c85:	83 c4 10             	add    esp,0x10
  104c88:	eb a5                	jmp    104c2f <isr_handler+0x1f>
  104c8a:	66 90                	xchg   ax,ax
  104c8c:	66 90                	xchg   ax,ax
  104c8e:	66 90                	xchg   ax,ax

00104c90 <pic_setup>:
#define ICW1_ICW4      0x01
#define ICW1_INIT      0x10


void pic_setup()
{
  104c90:	83 ec 14             	sub    esp,0x14
	/* Cascade initialization */
	outb(PIC1_COMMAND, ICW1_INIT|ICW1_ICW4); PIC_WAIT();
  104c93:	6a 11                	push   0x11
  104c95:	6a 20                	push   0x20
  104c97:	e8 74 f7 ff ff       	call   104410 <outb>
  104c9c:	eb 00                	jmp    104c9e <pic_setup+0xe>
  104c9e:	eb 00                	jmp    104ca0 <pic_setup+0x10>
	outb(PIC2_COMMAND, ICW1_INIT|ICW1_ICW4); PIC_WAIT();
  104ca0:	58                   	pop    eax
  104ca1:	5a                   	pop    edx
  104ca2:	6a 11                	push   0x11
  104ca4:	68 a0 00 00 00       	push   0xa0
  104ca9:	e8 62 f7 ff ff       	call   104410 <outb>
  104cae:	eb 00                	jmp    104cb0 <pic_setup+0x20>
  104cb0:	eb 00                	jmp    104cb2 <pic_setup+0x22>

	/* Remap */
	outb(PIC1_DATA, PIC1_OFFSET); PIC_WAIT();
  104cb2:	59                   	pop    ecx
  104cb3:	58                   	pop    eax
  104cb4:	6a 20                	push   0x20
  104cb6:	6a 21                	push   0x21
  104cb8:	e8 53 f7 ff ff       	call   104410 <outb>
  104cbd:	eb 00                	jmp    104cbf <pic_setup+0x2f>
  104cbf:	eb 00                	jmp    104cc1 <pic_setup+0x31>
	outb(PIC2_DATA, PIC2_OFFSET); PIC_WAIT();
  104cc1:	58                   	pop    eax
  104cc2:	5a                   	pop    edx
  104cc3:	6a 28                	push   0x28
  104cc5:	68 a1 00 00 00       	push   0xa1
  104cca:	e8 41 f7 ff ff       	call   104410 <outb>
  104ccf:	eb 00                	jmp    104cd1 <pic_setup+0x41>
  104cd1:	eb 00                	jmp    104cd3 <pic_setup+0x43>

	/* Cascade identity with slave PIC at IRQ2 */
	outb(PIC1_DATA, 0x04); PIC_WAIT();
  104cd3:	59                   	pop    ecx
  104cd4:	58                   	pop    eax
  104cd5:	6a 04                	push   0x4
  104cd7:	6a 21                	push   0x21
  104cd9:	e8 32 f7 ff ff       	call   104410 <outb>
  104cde:	eb 00                	jmp    104ce0 <pic_setup+0x50>
  104ce0:	eb 00                	jmp    104ce2 <pic_setup+0x52>
	outb(PIC2_DATA, 0x02); PIC_WAIT();
  104ce2:	58                   	pop    eax
  104ce3:	5a                   	pop    edx
  104ce4:	6a 02                	push   0x2
  104ce6:	68 a1 00 00 00       	push   0xa1
  104ceb:	e8 20 f7 ff ff       	call   104410 <outb>
  104cf0:	eb 00                	jmp    104cf2 <pic_setup+0x62>
  104cf2:	eb 00                	jmp    104cf4 <pic_setup+0x64>

	/* Request 8086 mode on each PIC */
	outb(PIC1_DATA, 0x01); PIC_WAIT();
  104cf4:	59                   	pop    ecx
  104cf5:	58                   	pop    eax
  104cf6:	6a 01                	push   0x1
  104cf8:	6a 21                	push   0x21
  104cfa:	e8 11 f7 ff ff       	call   104410 <outb>
  104cff:	eb 00                	jmp    104d01 <pic_setup+0x71>
  104d01:	eb 00                	jmp    104d03 <pic_setup+0x73>
	outb(PIC2_DATA, 0x01); PIC_WAIT();
  104d03:	58                   	pop    eax
  104d04:	5a                   	pop    edx
  104d05:	6a 01                	push   0x1
  104d07:	68 a1 00 00 00       	push   0xa1
  104d0c:	e8 ff f6 ff ff       	call   104410 <outb>
  104d11:	eb 00                	jmp    104d13 <pic_setup+0x83>
  104d13:	eb 00                	jmp    104d15 <pic_setup+0x85>
  104d15:	83 c4 1c             	add    esp,0x1c
  104d18:	c3                   	ret    
  104d19:	66 90                	xchg   ax,ax
  104d1b:	66 90                	xchg   ax,ax
  104d1d:	66 90                	xchg   ax,ax
  104d1f:	90                   	nop

00104d20 <is_cmos_update>:
#define CMOS_DATA 0x71

#define from_bcd(val)  ((val / 16) * 10 + (val & 0xf))

bool is_cmos_update()
{
  104d20:	83 ec 14             	sub    esp,0x14
    outb(CMOS_ADDRESS, 0x0A);
  104d23:	6a 0a                	push   0xa
  104d25:	6a 70                	push   0x70
  104d27:	e8 e4 f6 ff ff       	call   104410 <outb>
    return (inb(CMOS_DATA) & 0x80);
  104d2c:	c7 04 24 71 00 00 00 	mov    DWORD PTR [esp],0x71
  104d33:	e8 c8 f6 ff ff       	call   104400 <inb>
}
  104d38:	83 c4 1c             	add    esp,0x1c
    return (inb(CMOS_DATA) & 0x80);
  104d3b:	83 e0 80             	and    eax,0xffffff80
}
  104d3e:	c3                   	ret    
  104d3f:	90                   	nop

00104d40 <get_realtime_reg>:

u8 get_realtime_reg(s32 reg)
{
  104d40:	83 ec 14             	sub    esp,0x14
    outb(CMOS_ADDRESS, reg);
  104d43:	0f b6 44 24 18       	movzx  eax,BYTE PTR [esp+0x18]
  104d48:	50                   	push   eax
  104d49:	6a 70                	push   0x70
  104d4b:	e8 c0 f6 ff ff       	call   104410 <outb>
    return inb(CMOS_DATA);
  104d50:	c7 44 24 20 71 00 00 	mov    DWORD PTR [esp+0x20],0x71
  104d57:	00 
}
  104d58:	83 c4 1c             	add    esp,0x1c
    return inb(CMOS_DATA);
  104d5b:	e9 a0 f6 ff ff       	jmp    104400 <inb>

00104d60 <time>:

time_t time()
{
  104d60:	55                   	push   ebp
  104d61:	57                   	push   edi
  104d62:	56                   	push   esi
  104d63:	53                   	push   ebx
  104d64:	83 ec 1c             	sub    esp,0x1c
  104d67:	8b 6c 24 30          	mov    ebp,DWORD PTR [esp+0x30]
  104d6b:	90                   	nop
  104d6c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
    outb(CMOS_ADDRESS, 0x0A);
  104d70:	83 ec 08             	sub    esp,0x8
  104d73:	6a 0a                	push   0xa
  104d75:	6a 70                	push   0x70
  104d77:	e8 94 f6 ff ff       	call   104410 <outb>
    return (inb(CMOS_DATA) & 0x80);
  104d7c:	c7 04 24 71 00 00 00 	mov    DWORD PTR [esp],0x71
  104d83:	e8 78 f6 ff ff       	call   104400 <inb>
    // Waite for cmos update.
    while (is_cmos_update());
  104d88:	83 c4 10             	add    esp,0x10
  104d8b:	84 c0                	test   al,al
  104d8d:	78 e1                	js     104d70 <time+0x10>
    outb(CMOS_ADDRESS, reg);
  104d8f:	83 ec 08             	sub    esp,0x8
  104d92:	6a 00                	push   0x0
  104d94:	6a 70                	push   0x70
  104d96:	e8 75 f6 ff ff       	call   104410 <outb>
    return inb(CMOS_DATA);
  104d9b:	c7 04 24 71 00 00 00 	mov    DWORD PTR [esp],0x71
  104da2:	e8 59 f6 ff ff       	call   104400 <inb>
  104da7:	89 c3                	mov    ebx,eax
    outb(CMOS_ADDRESS, reg);
  104da9:	58                   	pop    eax
  104daa:	5a                   	pop    edx
  104dab:	6a 00                	push   0x0
  104dad:	6a 70                	push   0x70
    time_t time;

    time.second = from_bcd(get_realtime_reg(TIME_SECOND));
  104daf:	c0 eb 04             	shr    bl,0x4
    outb(CMOS_ADDRESS, reg);
  104db2:	e8 59 f6 ff ff       	call   104410 <outb>
    return inb(CMOS_DATA);
  104db7:	c7 04 24 71 00 00 00 	mov    DWORD PTR [esp],0x71
  104dbe:	e8 3d f6 ff ff       	call   104400 <inb>
    time.second = from_bcd(get_realtime_reg(TIME_SECOND));
  104dc3:	8d 14 9b             	lea    edx,[ebx+ebx*4]
  104dc6:	83 e0 0f             	and    eax,0xf
  104dc9:	8d 0c 50             	lea    ecx,[eax+edx*2]
  104dcc:	88 4c 24 1f          	mov    BYTE PTR [esp+0x1f],cl
    outb(CMOS_ADDRESS, reg);
  104dd0:	59                   	pop    ecx
  104dd1:	5b                   	pop    ebx
  104dd2:	6a 02                	push   0x2
  104dd4:	6a 70                	push   0x70
  104dd6:	e8 35 f6 ff ff       	call   104410 <outb>
    return inb(CMOS_DATA);
  104ddb:	c7 04 24 71 00 00 00 	mov    DWORD PTR [esp],0x71
  104de2:	e8 19 f6 ff ff       	call   104400 <inb>
    outb(CMOS_ADDRESS, reg);
  104de7:	5e                   	pop    esi
  104de8:	5f                   	pop    edi
    return inb(CMOS_DATA);
  104de9:	89 c3                	mov    ebx,eax
    outb(CMOS_ADDRESS, reg);
  104deb:	6a 02                	push   0x2
  104ded:	6a 70                	push   0x70
    time.minute = from_bcd(get_realtime_reg(TIME_MINUTE));
  104def:	c0 eb 04             	shr    bl,0x4
    outb(CMOS_ADDRESS, reg);
  104df2:	e8 19 f6 ff ff       	call   104410 <outb>
    return inb(CMOS_DATA);
  104df7:	c7 04 24 71 00 00 00 	mov    DWORD PTR [esp],0x71
  104dfe:	e8 fd f5 ff ff       	call   104400 <inb>
    time.minute = from_bcd(get_realtime_reg(TIME_MINUTE));
  104e03:	8d 14 9b             	lea    edx,[ebx+ebx*4]
  104e06:	83 e0 0f             	and    eax,0xf
  104e09:	8d 04 50             	lea    eax,[eax+edx*2]
  104e0c:	88 44 24 18          	mov    BYTE PTR [esp+0x18],al
    outb(CMOS_ADDRESS, reg);
  104e10:	58                   	pop    eax
  104e11:	5a                   	pop    edx
  104e12:	6a 04                	push   0x4
  104e14:	6a 70                	push   0x70
  104e16:	e8 f5 f5 ff ff       	call   104410 <outb>
    return inb(CMOS_DATA);
  104e1b:	c7 04 24 71 00 00 00 	mov    DWORD PTR [esp],0x71
  104e22:	e8 d9 f5 ff ff       	call   104400 <inb>
    outb(CMOS_ADDRESS, reg);
  104e27:	59                   	pop    ecx
  104e28:	5e                   	pop    esi
    return inb(CMOS_DATA);
  104e29:	89 c3                	mov    ebx,eax
    outb(CMOS_ADDRESS, reg);
  104e2b:	6a 04                	push   0x4
  104e2d:	6a 70                	push   0x70
    time.hour = from_bcd(get_realtime_reg(TIME_HOUR));
  104e2f:	c0 eb 04             	shr    bl,0x4
    outb(CMOS_ADDRESS, reg);
  104e32:	e8 d9 f5 ff ff       	call   104410 <outb>
    return inb(CMOS_DATA);
  104e37:	c7 04 24 71 00 00 00 	mov    DWORD PTR [esp],0x71
  104e3e:	e8 bd f5 ff ff       	call   104400 <inb>
    time.hour = from_bcd(get_realtime_reg(TIME_HOUR));
  104e43:	8d 14 9b             	lea    edx,[ebx+ebx*4]
  104e46:	83 e0 0f             	and    eax,0xf
    outb(CMOS_ADDRESS, reg);
  104e49:	5f                   	pop    edi
    time.hour = from_bcd(get_realtime_reg(TIME_HOUR));
  104e4a:	8d 1c 50             	lea    ebx,[eax+edx*2]
    outb(CMOS_ADDRESS, reg);
  104e4d:	58                   	pop    eax
  104e4e:	6a 07                	push   0x7
  104e50:	6a 70                	push   0x70
    time.day = from_bcd(get_realtime_reg(TIME_DAY));
    time.month = from_bcd(get_realtime_reg(TIME_MONTH));
    time.year = from_bcd(get_realtime_reg(TIME_YEAR));

    return time;
  104e52:	0f b6 db             	movzx  ebx,bl
    outb(CMOS_ADDRESS, reg);
  104e55:	e8 b6 f5 ff ff       	call   104410 <outb>
    return inb(CMOS_DATA);
  104e5a:	c7 04 24 71 00 00 00 	mov    DWORD PTR [esp],0x71
  104e61:	e8 9a f5 ff ff       	call   104400 <inb>
  104e66:	89 c7                	mov    edi,eax
    outb(CMOS_ADDRESS, reg);
  104e68:	58                   	pop    eax
  104e69:	5a                   	pop    edx
  104e6a:	6a 07                	push   0x7
  104e6c:	6a 70                	push   0x70
  104e6e:	e8 9d f5 ff ff       	call   104410 <outb>
    return inb(CMOS_DATA);
  104e73:	c7 04 24 71 00 00 00 	mov    DWORD PTR [esp],0x71
  104e7a:	e8 81 f5 ff ff       	call   104400 <inb>
    time.day = from_bcd(get_realtime_reg(TIME_DAY));
  104e7f:	89 f9                	mov    ecx,edi
  104e81:	83 e0 0f             	and    eax,0xf
  104e84:	c0 e9 04             	shr    cl,0x4
  104e87:	89 cf                	mov    edi,ecx
  104e89:	8d 3c bf             	lea    edi,[edi+edi*4]
    outb(CMOS_ADDRESS, reg);
  104e8c:	59                   	pop    ecx
    time.day = from_bcd(get_realtime_reg(TIME_DAY));
  104e8d:	8d 34 78             	lea    esi,[eax+edi*2]
    outb(CMOS_ADDRESS, reg);
  104e90:	5f                   	pop    edi
  104e91:	6a 08                	push   0x8
  104e93:	6a 70                	push   0x70
  104e95:	e8 76 f5 ff ff       	call   104410 <outb>
    return inb(CMOS_DATA);
  104e9a:	c7 04 24 71 00 00 00 	mov    DWORD PTR [esp],0x71
  104ea1:	e8 5a f5 ff ff       	call   104400 <inb>
  104ea6:	89 c7                	mov    edi,eax
    outb(CMOS_ADDRESS, reg);
  104ea8:	58                   	pop    eax
  104ea9:	5a                   	pop    edx
  104eaa:	6a 08                	push   0x8
  104eac:	6a 70                	push   0x70
  104eae:	e8 5d f5 ff ff       	call   104410 <outb>
    return inb(CMOS_DATA);
  104eb3:	c7 04 24 71 00 00 00 	mov    DWORD PTR [esp],0x71
  104eba:	e8 41 f5 ff ff       	call   104400 <inb>
    time.month = from_bcd(get_realtime_reg(TIME_MONTH));
  104ebf:	89 f9                	mov    ecx,edi
  104ec1:	83 e0 0f             	and    eax,0xf
  104ec4:	c0 e9 04             	shr    cl,0x4
  104ec7:	89 cf                	mov    edi,ecx
  104ec9:	8d 3c bf             	lea    edi,[edi+edi*4]
    outb(CMOS_ADDRESS, reg);
  104ecc:	59                   	pop    ecx
    time.month = from_bcd(get_realtime_reg(TIME_MONTH));
  104ecd:	8d 3c 78             	lea    edi,[eax+edi*2]
    outb(CMOS_ADDRESS, reg);
  104ed0:	58                   	pop    eax
  104ed1:	6a 09                	push   0x9
  104ed3:	6a 70                	push   0x70
  104ed5:	e8 36 f5 ff ff       	call   104410 <outb>
    return inb(CMOS_DATA);
  104eda:	c7 04 24 71 00 00 00 	mov    DWORD PTR [esp],0x71
  104ee1:	e8 1a f5 ff ff       	call   104400 <inb>
  104ee6:	88 44 24 1e          	mov    BYTE PTR [esp+0x1e],al
    return time;
  104eea:	c1 e3 10             	shl    ebx,0x10
  104eed:	c1 e6 18             	shl    esi,0x18
    outb(CMOS_ADDRESS, reg);
  104ef0:	58                   	pop    eax
  104ef1:	5a                   	pop    edx
  104ef2:	6a 09                	push   0x9
  104ef4:	6a 70                	push   0x70
  104ef6:	e8 15 f5 ff ff       	call   104410 <outb>
    return inb(CMOS_DATA);
  104efb:	c7 04 24 71 00 00 00 	mov    DWORD PTR [esp],0x71
  104f02:	e8 f9 f4 ff ff       	call   104400 <inb>
    return time;
  104f07:	0f b6 4c 24 1f       	movzx  ecx,BYTE PTR [esp+0x1f]
  104f0c:	31 d2                	xor    edx,edx
    time.year = from_bcd(get_realtime_reg(TIME_YEAR));
  104f0e:	83 e0 0f             	and    eax,0xf
    return time;
  104f11:	88 ca                	mov    dl,cl
  104f13:	8a 74 24 18          	mov    dh,BYTE PTR [esp+0x18]
  104f17:	89 f9                	mov    ecx,edi
  104f19:	88 4d 04             	mov    BYTE PTR [ebp+0x4],cl
  104f1c:	0f b7 d2             	movzx  edx,dx
  104f1f:	09 da                	or     edx,ebx
  104f21:	09 d6                	or     esi,edx
    time.year = from_bcd(get_realtime_reg(TIME_YEAR));
  104f23:	0f b6 54 24 1e       	movzx  edx,BYTE PTR [esp+0x1e]
    return time;
  104f28:	89 75 00             	mov    DWORD PTR [ebp+0x0],esi
    time.year = from_bcd(get_realtime_reg(TIME_YEAR));
  104f2b:	c0 ea 04             	shr    dl,0x4
  104f2e:	0f b6 d2             	movzx  edx,dl
  104f31:	8d 14 92             	lea    edx,[edx+edx*4]
  104f34:	8d 04 50             	lea    eax,[eax+edx*2]
  104f37:	89 45 08             	mov    DWORD PTR [ebp+0x8],eax
}
  104f3a:	83 c4 2c             	add    esp,0x2c
  104f3d:	89 e8                	mov    eax,ebp
  104f3f:	5b                   	pop    ebx
  104f40:	5e                   	pop    esi
  104f41:	5f                   	pop    edi
  104f42:	5d                   	pop    ebp
  104f43:	c2 04 00             	ret    0x4
  104f46:	8d 76 00             	lea    esi,[esi+0x0]
  104f49:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00104f50 <time_get>:

u32 time_get(u32 selector)
{
  104f50:	56                   	push   esi
  104f51:	53                   	push   ebx
  104f52:	83 ec 04             	sub    esp,0x4
  104f55:	8d 76 00             	lea    esi,[esi+0x0]
    outb(CMOS_ADDRESS, 0x0A);
  104f58:	83 ec 08             	sub    esp,0x8
  104f5b:	6a 0a                	push   0xa
  104f5d:	6a 70                	push   0x70
  104f5f:	e8 ac f4 ff ff       	call   104410 <outb>
    return (inb(CMOS_DATA) & 0x80);
  104f64:	c7 04 24 71 00 00 00 	mov    DWORD PTR [esp],0x71
  104f6b:	e8 90 f4 ff ff       	call   104400 <inb>
    while (is_cmos_update());
  104f70:	83 c4 10             	add    esp,0x10
  104f73:	84 c0                	test   al,al
  104f75:	78 e1                	js     104f58 <time_get+0x8>
  104f77:	0f b6 74 24 10       	movzx  esi,BYTE PTR [esp+0x10]
    outb(CMOS_ADDRESS, reg);
  104f7c:	83 ec 08             	sub    esp,0x8
  104f7f:	56                   	push   esi
  104f80:	6a 70                	push   0x70
  104f82:	e8 89 f4 ff ff       	call   104410 <outb>
    return inb(CMOS_DATA);
  104f87:	c7 04 24 71 00 00 00 	mov    DWORD PTR [esp],0x71
  104f8e:	e8 6d f4 ff ff       	call   104400 <inb>
  104f93:	89 c3                	mov    ebx,eax
    outb(CMOS_ADDRESS, reg);
  104f95:	58                   	pop    eax
  104f96:	5a                   	pop    edx
  104f97:	56                   	push   esi
  104f98:	6a 70                	push   0x70
    return from_bcd(get_realtime_reg(selector));
  104f9a:	c0 eb 04             	shr    bl,0x4
    outb(CMOS_ADDRESS, reg);
  104f9d:	e8 6e f4 ff ff       	call   104410 <outb>
    return inb(CMOS_DATA);
  104fa2:	c7 04 24 71 00 00 00 	mov    DWORD PTR [esp],0x71
  104fa9:	e8 52 f4 ff ff       	call   104400 <inb>
    return from_bcd(get_realtime_reg(selector));
  104fae:	0f b6 d3             	movzx  edx,bl
}
  104fb1:	83 c4 14             	add    esp,0x14
    return from_bcd(get_realtime_reg(selector));
  104fb4:	83 e0 0f             	and    eax,0xf
  104fb7:	8d 14 92             	lea    edx,[edx+edx*4]
}
  104fba:	5b                   	pop    ebx
    return from_bcd(get_realtime_reg(selector));
  104fbb:	8d 04 50             	lea    eax,[eax+edx*2]
}
  104fbe:	5e                   	pop    esi
  104fbf:	c3                   	ret    

00104fc0 <time_task>:
#include <string.h>
#include "devices/vga.h"


void time_task()
{
  104fc0:	55                   	push   ebp
  104fc1:	57                   	push   edi
  104fc2:	56                   	push   esi
  104fc3:	53                   	push   ebx
  104fc4:	83 ec 5c             	sub    esp,0x5c
  104fc7:	89 e3                	mov    ebx,esp
  104fc9:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
    char buffer[80];
    while(true)
    {
        memset(buffer, 0, 80);
  104fd0:	83 ec 04             	sub    esp,0x4
  104fd3:	6a 50                	push   0x50
  104fd5:	6a 00                	push   0x0
  104fd7:	53                   	push   ebx
  104fd8:	e8 f3 06 00 00       	call   1056d0 <memset>
  104fdd:	83 c4 10             	add    esp,0x10
    outb(CMOS_ADDRESS, 0x0A);
  104fe0:	83 ec 08             	sub    esp,0x8
  104fe3:	6a 0a                	push   0xa
  104fe5:	6a 70                	push   0x70
  104fe7:	e8 24 f4 ff ff       	call   104410 <outb>
    return (inb(CMOS_DATA) & 0x80);
  104fec:	c7 04 24 71 00 00 00 	mov    DWORD PTR [esp],0x71
  104ff3:	e8 08 f4 ff ff       	call   104400 <inb>
    while (is_cmos_update());
  104ff8:	83 c4 10             	add    esp,0x10
  104ffb:	84 c0                	test   al,al
  104ffd:	78 e1                	js     104fe0 <time_task+0x20>
    outb(CMOS_ADDRESS, reg);
  104fff:	83 ec 08             	sub    esp,0x8
  105002:	6a 00                	push   0x0
  105004:	6a 70                	push   0x70
  105006:	e8 05 f4 ff ff       	call   104410 <outb>
    return inb(CMOS_DATA);
  10500b:	c7 04 24 71 00 00 00 	mov    DWORD PTR [esp],0x71
  105012:	e8 e9 f3 ff ff       	call   104400 <inb>
  105017:	89 c6                	mov    esi,eax
    outb(CMOS_ADDRESS, reg);
  105019:	58                   	pop    eax
  10501a:	5a                   	pop    edx
  10501b:	6a 00                	push   0x0
  10501d:	6a 70                	push   0x70
  10501f:	e8 ec f3 ff ff       	call   104410 <outb>
    return inb(CMOS_DATA);
  105024:	c7 04 24 71 00 00 00 	mov    DWORD PTR [esp],0x71
  10502b:	e8 d0 f3 ff ff       	call   104400 <inb>
    return from_bcd(get_realtime_reg(selector));
  105030:	89 f2                	mov    edx,esi
  105032:	83 e0 0f             	and    eax,0xf
  105035:	83 c4 10             	add    esp,0x10
  105038:	c0 ea 04             	shr    dl,0x4
  10503b:	0f b6 d2             	movzx  edx,dl
  10503e:	8d 14 92             	lea    edx,[edx+edx*4]
  105041:	8d 34 50             	lea    esi,[eax+edx*2]
  105044:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
    outb(CMOS_ADDRESS, 0x0A);
  105048:	83 ec 08             	sub    esp,0x8
  10504b:	6a 0a                	push   0xa
  10504d:	6a 70                	push   0x70
  10504f:	e8 bc f3 ff ff       	call   104410 <outb>
    return (inb(CMOS_DATA) & 0x80);
  105054:	c7 04 24 71 00 00 00 	mov    DWORD PTR [esp],0x71
  10505b:	e8 a0 f3 ff ff       	call   104400 <inb>
    while (is_cmos_update());
  105060:	83 c4 10             	add    esp,0x10
  105063:	84 c0                	test   al,al
  105065:	78 e1                	js     105048 <time_task+0x88>
    outb(CMOS_ADDRESS, reg);
  105067:	83 ec 08             	sub    esp,0x8
  10506a:	6a 02                	push   0x2
  10506c:	6a 70                	push   0x70
  10506e:	e8 9d f3 ff ff       	call   104410 <outb>
    return inb(CMOS_DATA);
  105073:	c7 04 24 71 00 00 00 	mov    DWORD PTR [esp],0x71
  10507a:	e8 81 f3 ff ff       	call   104400 <inb>
    outb(CMOS_ADDRESS, reg);
  10507f:	59                   	pop    ecx
  105080:	5d                   	pop    ebp
  105081:	6a 02                	push   0x2
  105083:	6a 70                	push   0x70
    return inb(CMOS_DATA);
  105085:	89 c7                	mov    edi,eax
    outb(CMOS_ADDRESS, reg);
  105087:	e8 84 f3 ff ff       	call   104410 <outb>
    return inb(CMOS_DATA);
  10508c:	c7 04 24 71 00 00 00 	mov    DWORD PTR [esp],0x71
  105093:	e8 68 f3 ff ff       	call   104400 <inb>
    return from_bcd(get_realtime_reg(selector));
  105098:	89 fa                	mov    edx,edi
  10509a:	83 e0 0f             	and    eax,0xf
  10509d:	83 c4 10             	add    esp,0x10
  1050a0:	c0 ea 04             	shr    dl,0x4
  1050a3:	0f b6 d2             	movzx  edx,dl
  1050a6:	8d 14 92             	lea    edx,[edx+edx*4]
  1050a9:	8d 3c 50             	lea    edi,[eax+edx*2]
  1050ac:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
    outb(CMOS_ADDRESS, 0x0A);
  1050b0:	83 ec 08             	sub    esp,0x8
  1050b3:	6a 0a                	push   0xa
  1050b5:	6a 70                	push   0x70
  1050b7:	e8 54 f3 ff ff       	call   104410 <outb>
    return (inb(CMOS_DATA) & 0x80);
  1050bc:	c7 04 24 71 00 00 00 	mov    DWORD PTR [esp],0x71
  1050c3:	e8 38 f3 ff ff       	call   104400 <inb>
    while (is_cmos_update());
  1050c8:	83 c4 10             	add    esp,0x10
  1050cb:	84 c0                	test   al,al
  1050cd:	78 e1                	js     1050b0 <time_task+0xf0>
    outb(CMOS_ADDRESS, reg);
  1050cf:	83 ec 08             	sub    esp,0x8
  1050d2:	6a 04                	push   0x4
  1050d4:	6a 70                	push   0x70
  1050d6:	e8 35 f3 ff ff       	call   104410 <outb>
    return inb(CMOS_DATA);
  1050db:	c7 04 24 71 00 00 00 	mov    DWORD PTR [esp],0x71
  1050e2:	e8 19 f3 ff ff       	call   104400 <inb>
  1050e7:	89 c5                	mov    ebp,eax
    outb(CMOS_ADDRESS, reg);
  1050e9:	58                   	pop    eax
  1050ea:	5a                   	pop    edx
  1050eb:	6a 04                	push   0x4
  1050ed:	6a 70                	push   0x70
  1050ef:	e8 1c f3 ff ff       	call   104410 <outb>
    return inb(CMOS_DATA);
  1050f4:	c7 04 24 71 00 00 00 	mov    DWORD PTR [esp],0x71
  1050fb:	e8 00 f3 ff ff       	call   104400 <inb>
    return from_bcd(get_realtime_reg(selector));
  105100:	89 ea                	mov    edx,ebp
  105102:	83 e0 0f             	and    eax,0xf
        sprintf(buffer, " [ %s '%s' ] [ %d:%d:%d ]",
  105105:	83 c4 0c             	add    esp,0xc
    return from_bcd(get_realtime_reg(selector));
  105108:	c0 ea 04             	shr    dl,0x4
        sprintf(buffer, " [ %s '%s' ] [ %d:%d:%d ]",
  10510b:	56                   	push   esi
  10510c:	57                   	push   edi
    return from_bcd(get_realtime_reg(selector));
  10510d:	0f b6 d2             	movzx  edx,dl
        __kernel_name, __kernel_version_codename, 
        time_get(TIME_HOUR), time_get(TIME_MINUTE), time_get(TIME_SECOND));
    
        for(u32 i = 0; i < 80; i++)
  105110:	31 f6                	xor    esi,esi
    return from_bcd(get_realtime_reg(selector));
  105112:	8d 14 92             	lea    edx,[edx+edx*4]
  105115:	8d 04 50             	lea    eax,[eax+edx*2]
        sprintf(buffer, " [ %s '%s' ] [ %d:%d:%d ]",
  105118:	50                   	push   eax
  105119:	ff 35 08 70 11 00    	push   DWORD PTR ds:0x117008
  10511f:	ff 35 10 70 11 00    	push   DWORD PTR ds:0x117010
  105125:	68 71 24 11 00       	push   0x112471
  10512a:	53                   	push   ebx
  10512b:	e8 a0 19 00 00       	call   106ad0 <sprintf>
  105130:	83 c4 20             	add    esp,0x20
  105133:	90                   	nop
  105134:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
        {
            vga_cell(i, 0, vga_entry(buffer[i], vga_white, vga_gray));  
  105138:	66 0f be 04 33       	movsx  ax,BYTE PTR [ebx+esi*1]
  10513d:	83 ec 04             	sub    esp,0x4
  105140:	66 0d 00 8f          	or     ax,0x8f00
  105144:	0f b7 c0             	movzx  eax,ax
  105147:	50                   	push   eax
  105148:	6a 00                	push   0x0
  10514a:	56                   	push   esi
        for(u32 i = 0; i < 80; i++)
  10514b:	83 c6 01             	add    esi,0x1
            vga_cell(i, 0, vga_entry(buffer[i], vga_white, vga_gray));  
  10514e:	e8 7d f0 ff ff       	call   1041d0 <vga_cell>
        for(u32 i = 0; i < 80; i++)
  105153:	83 c4 10             	add    esp,0x10
  105156:	83 fe 50             	cmp    esi,0x50
  105159:	75 dd                	jne    105138 <time_task+0x178>
  10515b:	e9 70 fe ff ff       	jmp    104fd0 <time_task+0x10>

00105160 <irq_common>:
  105160:	60                   	pusha  
  105161:	1e                   	push   ds
  105162:	06                   	push   es
  105163:	0f a0                	push   fs
  105165:	0f a8                	push   gs
  105167:	66 b8 10 00          	mov    ax,0x10
  10516b:	8e d8                	mov    ds,eax
  10516d:	8e c0                	mov    es,eax
  10516f:	8e e0                	mov    fs,eax
  105171:	8e e8                	mov    gs,eax
  105173:	fc                   	cld    
  105174:	89 e0                	mov    eax,esp
  105176:	54                   	push   esp
  105177:	e8 24 f2 ff ff       	call   1043a0 <irq_handler>
  10517c:	89 c4                	mov    esp,eax
  10517e:	0f a9                	pop    gs
  105180:	0f a1                	pop    fs
  105182:	07                   	pop    es
  105183:	1f                   	pop    ds
  105184:	61                   	popa   
  105185:	83 c4 08             	add    esp,0x8
  105188:	cf                   	iret   

00105189 <irq0>:
  105189:	fa                   	cli    
  10518a:	6a 00                	push   0x0
  10518c:	6a 00                	push   0x0
  10518e:	eb d0                	jmp    105160 <irq_common>

00105190 <irq1>:
  105190:	fa                   	cli    
  105191:	6a 00                	push   0x0
  105193:	6a 01                	push   0x1
  105195:	eb c9                	jmp    105160 <irq_common>

00105197 <irq2>:
  105197:	fa                   	cli    
  105198:	6a 00                	push   0x0
  10519a:	6a 02                	push   0x2
  10519c:	eb c2                	jmp    105160 <irq_common>

0010519e <irq3>:
  10519e:	fa                   	cli    
  10519f:	6a 00                	push   0x0
  1051a1:	6a 03                	push   0x3
  1051a3:	eb bb                	jmp    105160 <irq_common>

001051a5 <irq4>:
  1051a5:	fa                   	cli    
  1051a6:	6a 00                	push   0x0
  1051a8:	6a 04                	push   0x4
  1051aa:	eb b4                	jmp    105160 <irq_common>

001051ac <irq5>:
  1051ac:	fa                   	cli    
  1051ad:	6a 00                	push   0x0
  1051af:	6a 05                	push   0x5
  1051b1:	eb ad                	jmp    105160 <irq_common>

001051b3 <irq6>:
  1051b3:	fa                   	cli    
  1051b4:	6a 00                	push   0x0
  1051b6:	6a 06                	push   0x6
  1051b8:	eb a6                	jmp    105160 <irq_common>

001051ba <irq7>:
  1051ba:	fa                   	cli    
  1051bb:	6a 00                	push   0x0
  1051bd:	6a 07                	push   0x7
  1051bf:	eb 9f                	jmp    105160 <irq_common>

001051c1 <irq8>:
  1051c1:	fa                   	cli    
  1051c2:	6a 00                	push   0x0
  1051c4:	6a 08                	push   0x8
  1051c6:	eb 98                	jmp    105160 <irq_common>

001051c8 <irq9>:
  1051c8:	fa                   	cli    
  1051c9:	6a 00                	push   0x0
  1051cb:	6a 09                	push   0x9
  1051cd:	eb 91                	jmp    105160 <irq_common>

001051cf <irq10>:
  1051cf:	fa                   	cli    
  1051d0:	6a 00                	push   0x0
  1051d2:	6a 0a                	push   0xa
  1051d4:	eb 8a                	jmp    105160 <irq_common>

001051d6 <irq11>:
  1051d6:	fa                   	cli    
  1051d7:	6a 00                	push   0x0
  1051d9:	6a 0b                	push   0xb
  1051db:	eb 83                	jmp    105160 <irq_common>

001051dd <irq12>:
  1051dd:	fa                   	cli    
  1051de:	6a 00                	push   0x0
  1051e0:	6a 0c                	push   0xc
  1051e2:	e9 79 ff ff ff       	jmp    105160 <irq_common>

001051e7 <irq13>:
  1051e7:	fa                   	cli    
  1051e8:	6a 00                	push   0x0
  1051ea:	6a 0d                	push   0xd
  1051ec:	e9 6f ff ff ff       	jmp    105160 <irq_common>

001051f1 <irq14>:
  1051f1:	fa                   	cli    
  1051f2:	6a 00                	push   0x0
  1051f4:	6a 0e                	push   0xe
  1051f6:	e9 65 ff ff ff       	jmp    105160 <irq_common>

001051fb <irq15>:
  1051fb:	fa                   	cli    
  1051fc:	6a 00                	push   0x0
  1051fe:	6a 0f                	push   0xf
  105200:	e9 5b ff ff ff       	jmp    105160 <irq_common>

00105205 <irq_vector>:
  105205:	89 51 10             	mov    DWORD PTR [ecx+0x10],edx
  105208:	00 90 51 10 00 97    	add    BYTE PTR [eax-0x68ffefaf],dl
  10520e:	51                   	push   ecx
  10520f:	10 00                	adc    BYTE PTR [eax],al
  105211:	9e                   	sahf   
  105212:	51                   	push   ecx
  105213:	10 00                	adc    BYTE PTR [eax],al
  105215:	a5                   	movs   DWORD PTR es:[edi],DWORD PTR ds:[esi]
  105216:	51                   	push   ecx
  105217:	10 00                	adc    BYTE PTR [eax],al
  105219:	ac                   	lods   al,BYTE PTR ds:[esi]
  10521a:	51                   	push   ecx
  10521b:	10 00                	adc    BYTE PTR [eax],al
  10521d:	b3 51                	mov    bl,0x51
  10521f:	10 00                	adc    BYTE PTR [eax],al
  105221:	ba 51 10 00 c1       	mov    edx,0xc1001051
  105226:	51                   	push   ecx
  105227:	10 00                	adc    BYTE PTR [eax],al
  105229:	c8 51 10 00          	enter  0x1051,0x0
  10522d:	cf                   	iret   
  10522e:	51                   	push   ecx
  10522f:	10 00                	adc    BYTE PTR [eax],al
  105231:	d6                   	(bad)  
  105232:	51                   	push   ecx
  105233:	10 00                	adc    BYTE PTR [eax],al
  105235:	dd 51 10             	fst    QWORD PTR [ecx+0x10]
  105238:	00 e7                	add    bh,ah
  10523a:	51                   	push   ecx
  10523b:	10 00                	adc    BYTE PTR [eax],al
  10523d:	f1                   	icebp  
  10523e:	51                   	push   ecx
  10523f:	10 00                	adc    BYTE PTR [eax],al
  105241:	fb                   	sti    
  105242:	51                   	push   ecx
  105243:	10 00                	adc    BYTE PTR [eax],al
  105245:	66 90                	xchg   ax,ax
  105247:	66 90                	xchg   ax,ax
  105249:	66 90                	xchg   ax,ax
  10524b:	66 90                	xchg   ax,ax
  10524d:	66 90                	xchg   ax,ax
  10524f:	90                   	nop

00105250 <isr_common>:
  105250:	60                   	pusha  
  105251:	1e                   	push   ds
  105252:	06                   	push   es
  105253:	0f a0                	push   fs
  105255:	0f a8                	push   gs
  105257:	66 b8 10 00          	mov    ax,0x10
  10525b:	8e d8                	mov    ds,eax
  10525d:	8e c0                	mov    es,eax
  10525f:	8e e0                	mov    fs,eax
  105261:	8e e8                	mov    gs,eax
  105263:	e8 a8 f9 ff ff       	call   104c10 <isr_handler>
  105268:	0f a9                	pop    gs
  10526a:	0f a1                	pop    fs
  10526c:	07                   	pop    es
  10526d:	1f                   	pop    ds
  10526e:	61                   	popa   
  10526f:	83 c4 08             	add    esp,0x8
  105272:	cf                   	iret   

00105273 <isr0>:
  105273:	fa                   	cli    
  105274:	6a 00                	push   0x0
  105276:	6a 00                	push   0x0
  105278:	eb d6                	jmp    105250 <isr_common>

0010527a <isr1>:
  10527a:	fa                   	cli    
  10527b:	6a 00                	push   0x0
  10527d:	6a 01                	push   0x1
  10527f:	eb cf                	jmp    105250 <isr_common>

00105281 <isr2>:
  105281:	fa                   	cli    
  105282:	6a 00                	push   0x0
  105284:	6a 02                	push   0x2
  105286:	eb c8                	jmp    105250 <isr_common>

00105288 <isr3>:
  105288:	fa                   	cli    
  105289:	6a 00                	push   0x0
  10528b:	6a 03                	push   0x3
  10528d:	eb c1                	jmp    105250 <isr_common>

0010528f <isr4>:
  10528f:	fa                   	cli    
  105290:	6a 00                	push   0x0
  105292:	6a 04                	push   0x4
  105294:	eb ba                	jmp    105250 <isr_common>

00105296 <isr5>:
  105296:	fa                   	cli    
  105297:	6a 00                	push   0x0
  105299:	6a 05                	push   0x5
  10529b:	eb b3                	jmp    105250 <isr_common>

0010529d <isr6>:
  10529d:	fa                   	cli    
  10529e:	6a 00                	push   0x0
  1052a0:	6a 06                	push   0x6
  1052a2:	eb ac                	jmp    105250 <isr_common>

001052a4 <isr7>:
  1052a4:	fa                   	cli    
  1052a5:	6a 00                	push   0x0
  1052a7:	6a 07                	push   0x7
  1052a9:	eb a5                	jmp    105250 <isr_common>

001052ab <isr8>:
  1052ab:	fa                   	cli    
  1052ac:	6a 08                	push   0x8
  1052ae:	eb a0                	jmp    105250 <isr_common>

001052b0 <isr9>:
  1052b0:	fa                   	cli    
  1052b1:	6a 00                	push   0x0
  1052b3:	6a 09                	push   0x9
  1052b5:	eb 99                	jmp    105250 <isr_common>

001052b7 <isr10>:
  1052b7:	fa                   	cli    
  1052b8:	6a 0a                	push   0xa
  1052ba:	eb 94                	jmp    105250 <isr_common>

001052bc <isr11>:
  1052bc:	fa                   	cli    
  1052bd:	6a 0b                	push   0xb
  1052bf:	eb 8f                	jmp    105250 <isr_common>

001052c1 <isr12>:
  1052c1:	fa                   	cli    
  1052c2:	6a 0c                	push   0xc
  1052c4:	eb 8a                	jmp    105250 <isr_common>

001052c6 <isr13>:
  1052c6:	fa                   	cli    
  1052c7:	6a 0d                	push   0xd
  1052c9:	eb 85                	jmp    105250 <isr_common>

001052cb <isr14>:
  1052cb:	fa                   	cli    
  1052cc:	6a 0e                	push   0xe
  1052ce:	eb 80                	jmp    105250 <isr_common>

001052d0 <isr15>:
  1052d0:	fa                   	cli    
  1052d1:	6a 00                	push   0x0
  1052d3:	6a 0f                	push   0xf
  1052d5:	e9 76 ff ff ff       	jmp    105250 <isr_common>

001052da <isr16>:
  1052da:	fa                   	cli    
  1052db:	6a 00                	push   0x0
  1052dd:	6a 10                	push   0x10
  1052df:	e9 6c ff ff ff       	jmp    105250 <isr_common>

001052e4 <isr17>:
  1052e4:	fa                   	cli    
  1052e5:	6a 00                	push   0x0
  1052e7:	6a 11                	push   0x11
  1052e9:	e9 62 ff ff ff       	jmp    105250 <isr_common>

001052ee <isr18>:
  1052ee:	fa                   	cli    
  1052ef:	6a 00                	push   0x0
  1052f1:	6a 12                	push   0x12
  1052f3:	e9 58 ff ff ff       	jmp    105250 <isr_common>

001052f8 <isr19>:
  1052f8:	fa                   	cli    
  1052f9:	6a 00                	push   0x0
  1052fb:	6a 13                	push   0x13
  1052fd:	e9 4e ff ff ff       	jmp    105250 <isr_common>

00105302 <isr20>:
  105302:	fa                   	cli    
  105303:	6a 00                	push   0x0
  105305:	6a 14                	push   0x14
  105307:	e9 44 ff ff ff       	jmp    105250 <isr_common>

0010530c <isr21>:
  10530c:	fa                   	cli    
  10530d:	6a 00                	push   0x0
  10530f:	6a 15                	push   0x15
  105311:	e9 3a ff ff ff       	jmp    105250 <isr_common>

00105316 <isr22>:
  105316:	fa                   	cli    
  105317:	6a 00                	push   0x0
  105319:	6a 16                	push   0x16
  10531b:	e9 30 ff ff ff       	jmp    105250 <isr_common>

00105320 <isr23>:
  105320:	fa                   	cli    
  105321:	6a 00                	push   0x0
  105323:	6a 17                	push   0x17
  105325:	e9 26 ff ff ff       	jmp    105250 <isr_common>

0010532a <isr24>:
  10532a:	fa                   	cli    
  10532b:	6a 00                	push   0x0
  10532d:	6a 18                	push   0x18
  10532f:	e9 1c ff ff ff       	jmp    105250 <isr_common>

00105334 <isr25>:
  105334:	fa                   	cli    
  105335:	6a 00                	push   0x0
  105337:	6a 19                	push   0x19
  105339:	e9 12 ff ff ff       	jmp    105250 <isr_common>

0010533e <isr26>:
  10533e:	fa                   	cli    
  10533f:	6a 00                	push   0x0
  105341:	6a 1a                	push   0x1a
  105343:	e9 08 ff ff ff       	jmp    105250 <isr_common>

00105348 <isr27>:
  105348:	fa                   	cli    
  105349:	6a 00                	push   0x0
  10534b:	6a 1b                	push   0x1b
  10534d:	e9 fe fe ff ff       	jmp    105250 <isr_common>

00105352 <isr28>:
  105352:	fa                   	cli    
  105353:	6a 00                	push   0x0
  105355:	6a 1c                	push   0x1c
  105357:	e9 f4 fe ff ff       	jmp    105250 <isr_common>

0010535c <isr29>:
  10535c:	fa                   	cli    
  10535d:	6a 00                	push   0x0
  10535f:	6a 1d                	push   0x1d
  105361:	e9 ea fe ff ff       	jmp    105250 <isr_common>

00105366 <isr30>:
  105366:	fa                   	cli    
  105367:	6a 00                	push   0x0
  105369:	6a 1e                	push   0x1e
  10536b:	e9 e0 fe ff ff       	jmp    105250 <isr_common>

00105370 <isr31>:
  105370:	fa                   	cli    
  105371:	6a 00                	push   0x0
  105373:	6a 1f                	push   0x1f
  105375:	e9 d6 fe ff ff       	jmp    105250 <isr_common>

0010537a <isr128>:
  10537a:	fa                   	cli    
  10537b:	6a 00                	push   0x0
  10537d:	68 80 00 00 00       	push   0x80
  105382:	e9 c9 fe ff ff       	jmp    105250 <isr_common>

00105387 <isr_vector>:
  105387:	73 52                	jae    1053db <isr_vector+0x54>
  105389:	10 00                	adc    BYTE PTR [eax],al
  10538b:	7a 52                	jp     1053df <isr_vector+0x58>
  10538d:	10 00                	adc    BYTE PTR [eax],al
  10538f:	81 52 10 00 88 52 10 	adc    DWORD PTR [edx+0x10],0x10528800
  105396:	00 8f 52 10 00 96    	add    BYTE PTR [edi-0x69ffefae],cl
  10539c:	52                   	push   edx
  10539d:	10 00                	adc    BYTE PTR [eax],al
  10539f:	9d                   	popf   
  1053a0:	52                   	push   edx
  1053a1:	10 00                	adc    BYTE PTR [eax],al
  1053a3:	a4                   	movs   BYTE PTR es:[edi],BYTE PTR ds:[esi]
  1053a4:	52                   	push   edx
  1053a5:	10 00                	adc    BYTE PTR [eax],al
  1053a7:	ab                   	stos   DWORD PTR es:[edi],eax
  1053a8:	52                   	push   edx
  1053a9:	10 00                	adc    BYTE PTR [eax],al
  1053ab:	b0 52                	mov    al,0x52
  1053ad:	10 00                	adc    BYTE PTR [eax],al
  1053af:	b7 52                	mov    bh,0x52
  1053b1:	10 00                	adc    BYTE PTR [eax],al
  1053b3:	bc 52 10 00 c1       	mov    esp,0xc1001052
  1053b8:	52                   	push   edx
  1053b9:	10 00                	adc    BYTE PTR [eax],al
  1053bb:	c6                   	(bad)  
  1053bc:	52                   	push   edx
  1053bd:	10 00                	adc    BYTE PTR [eax],al
  1053bf:	cb                   	retf   
  1053c0:	52                   	push   edx
  1053c1:	10 00                	adc    BYTE PTR [eax],al
  1053c3:	d0 52 10             	rcl    BYTE PTR [edx+0x10],1
  1053c6:	00 da                	add    dl,bl
  1053c8:	52                   	push   edx
  1053c9:	10 00                	adc    BYTE PTR [eax],al
  1053cb:	e4 52                	in     al,0x52
  1053cd:	10 00                	adc    BYTE PTR [eax],al
  1053cf:	ee                   	out    dx,al
  1053d0:	52                   	push   edx
  1053d1:	10 00                	adc    BYTE PTR [eax],al
  1053d3:	f8                   	clc    
  1053d4:	52                   	push   edx
  1053d5:	10 00                	adc    BYTE PTR [eax],al
  1053d7:	02 53 10             	add    dl,BYTE PTR [ebx+0x10]
  1053da:	00 0c 53             	add    BYTE PTR [ebx+edx*2],cl
  1053dd:	10 00                	adc    BYTE PTR [eax],al
  1053df:	16                   	push   ss
  1053e0:	53                   	push   ebx
  1053e1:	10 00                	adc    BYTE PTR [eax],al
  1053e3:	20 53 10             	and    BYTE PTR [ebx+0x10],dl
  1053e6:	00 2a                	add    BYTE PTR [edx],ch
  1053e8:	53                   	push   ebx
  1053e9:	10 00                	adc    BYTE PTR [eax],al
  1053eb:	34 53                	xor    al,0x53
  1053ed:	10 00                	adc    BYTE PTR [eax],al
  1053ef:	3e 53                	ds push ebx
  1053f1:	10 00                	adc    BYTE PTR [eax],al
  1053f3:	48                   	dec    eax
  1053f4:	53                   	push   ebx
  1053f5:	10 00                	adc    BYTE PTR [eax],al
  1053f7:	52                   	push   edx
  1053f8:	53                   	push   ebx
  1053f9:	10 00                	adc    BYTE PTR [eax],al
  1053fb:	5c                   	pop    esp
  1053fc:	53                   	push   ebx
  1053fd:	10 00                	adc    BYTE PTR [eax],al
  1053ff:	66 53                	push   bx
  105401:	10 00                	adc    BYTE PTR [eax],al
  105403:	70 53                	jo     105458 <cpuid_get_feature_EDX+0x8>
  105405:	10 00                	adc    BYTE PTR [eax],al
  105407:	7a 53                	jp     10545c <cpuid_get_feature_ECX+0x2>
  105409:	10 00                	adc    BYTE PTR [eax],al
  10540b:	66 90                	xchg   ax,ax
  10540d:	66 90                	xchg   ax,ax
  10540f:	90                   	nop

00105410 <paging_enable>:
  105410:	0f 20 c0             	mov    eax,cr0
  105413:	0d 00 00 00 80       	or     eax,0x80000000
  105418:	0f 22 c0             	mov    cr0,eax
  10541b:	c3                   	ret    

0010541c <paging_load_directorie>:
  10541c:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  105420:	0f 22 d8             	mov    cr3,eax
  105423:	c3                   	ret    

00105424 <paging_invalidate_tlb>:
  105424:	0f 20 d8             	mov    eax,cr3
  105427:	0f 22 d8             	mov    cr3,eax
  10542a:	c3                   	ret    

0010542b <gdt_flush>:
  10542b:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  10542f:	0f 01 10             	lgdtd  [eax]
  105432:	66 b8 10 00          	mov    ax,0x10
  105436:	8e d8                	mov    ds,eax
  105438:	8e c0                	mov    es,eax
  10543a:	8e e0                	mov    fs,eax
  10543c:	8e d0                	mov    ss,eax
  10543e:	8e e8                	mov    gs,eax
  105440:	ea 47 54 10 00 08 00 	jmp    0x8:0x105447

00105447 <gdt_flush._gdt_flush>:
  105447:	c3                   	ret    

00105448 <load_idt>:
  105448:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  10544c:	0f 01 18             	lidtd  [eax]
  10544f:	c3                   	ret    

00105450 <cpuid_get_feature_EDX>:
  105450:	b8 01 00 00 00       	mov    eax,0x1
  105455:	0f a2                	cpuid  
  105457:	89 d0                	mov    eax,edx
  105459:	c3                   	ret    

0010545a <cpuid_get_feature_ECX>:
  10545a:	b8 01 00 00 00       	mov    eax,0x1
  10545f:	0f a2                	cpuid  
  105461:	89 c8                	mov    eax,ecx
  105463:	c3                   	ret    
  105464:	66 90                	xchg   ax,ax
  105466:	66 90                	xchg   ax,ax
  105468:	66 90                	xchg   ax,ax
  10546a:	66 90                	xchg   ax,ax
  10546c:	66 90                	xchg   ax,ax
  10546e:	66 90                	xchg   ax,ax

00105470 <_start>:
  105470:	fc                   	cld    
  105471:	bc 30 ee 23 00       	mov    esp,0x23ee30
  105476:	50                   	push   eax
  105477:	53                   	push   ebx
  105478:	e8 a3 33 00 00       	call   108820 <main>
  10547d:	fa                   	cli    

0010547e <_start.hang>:
  10547e:	f4                   	hlt    
  10547f:	eb fd                	jmp    10547e <_start.hang>

00105481 <_start.end>:
  105481:	66 90                	xchg   ax,ax
  105483:	66 90                	xchg   ax,ax
  105485:	66 90                	xchg   ax,ax
  105487:	66 90                	xchg   ax,ax
  105489:	66 90                	xchg   ax,ax
  10548b:	66 90                	xchg   ax,ax
  10548d:	66 90                	xchg   ax,ax
  10548f:	90                   	nop

00105490 <memchr>:
#include "string.h"
#include "math.h"

void *memchr(const void *str, int c, size_t n)
{
  105490:	56                   	push   esi
  105491:	53                   	push   ebx
  105492:	8b 5c 24 14          	mov    ebx,DWORD PTR [esp+0x14]
  105496:	8b 44 24 0c          	mov    eax,DWORD PTR [esp+0xc]
  10549a:	8b 74 24 10          	mov    esi,DWORD PTR [esp+0x10]
    unsigned char *s = (unsigned char *)str;

    for (size_t i = 0; i < n; i++)
  10549e:	85 db                	test   ebx,ebx
  1054a0:	74 1e                	je     1054c0 <memchr+0x30>
    {
        if (*(s + i) == c)
  1054a2:	0f b6 10             	movzx  edx,BYTE PTR [eax]
  1054a5:	39 d6                	cmp    esi,edx
  1054a7:	74 19                	je     1054c2 <memchr+0x32>
  1054a9:	8d 50 01             	lea    edx,[eax+0x1]
  1054ac:	01 c3                	add    ebx,eax
  1054ae:	eb 0c                	jmp    1054bc <memchr+0x2c>
  1054b0:	89 d0                	mov    eax,edx
  1054b2:	83 c2 01             	add    edx,0x1
  1054b5:	0f b6 08             	movzx  ecx,BYTE PTR [eax]
  1054b8:	39 f1                	cmp    ecx,esi
  1054ba:	74 06                	je     1054c2 <memchr+0x32>
    for (size_t i = 0; i < n; i++)
  1054bc:	39 da                	cmp    edx,ebx
  1054be:	75 f0                	jne    1054b0 <memchr+0x20>
        {
            return (s + i);
        }
    }

    return NULL;
  1054c0:	31 c0                	xor    eax,eax
}
  1054c2:	5b                   	pop    ebx
  1054c3:	5e                   	pop    esi
  1054c4:	c3                   	ret    
  1054c5:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
  1054c9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

001054d0 <memcmp>:

int memcmp(const void *str1, const void *str2, size_t n)
{
  1054d0:	57                   	push   edi
  1054d1:	56                   	push   esi
  1054d2:	53                   	push   ebx
  1054d3:	8b 5c 24 18          	mov    ebx,DWORD PTR [esp+0x18]
  1054d7:	8b 74 24 10          	mov    esi,DWORD PTR [esp+0x10]
  1054db:	8b 7c 24 14          	mov    edi,DWORD PTR [esp+0x14]
    const unsigned char *s1 = str1;
    const unsigned char *s2 = str2;

    for (size_t i = 0; i < n; i++)
  1054df:	85 db                	test   ebx,ebx
  1054e1:	74 28                	je     10550b <memcmp+0x3b>
    {
        if (*(s1 + i) != *(s2 + i))
  1054e3:	0f b6 16             	movzx  edx,BYTE PTR [esi]
  1054e6:	0f b6 0f             	movzx  ecx,BYTE PTR [edi]
  1054e9:	38 ca                	cmp    dl,cl
  1054eb:	75 2b                	jne    105518 <memcmp+0x48>
    for (size_t i = 0; i < n; i++)
  1054ed:	31 c0                	xor    eax,eax
  1054ef:	eb 13                	jmp    105504 <memcmp+0x34>
  1054f1:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
        if (*(s1 + i) != *(s2 + i))
  1054f8:	0f b6 14 06          	movzx  edx,BYTE PTR [esi+eax*1]
  1054fc:	0f b6 0c 07          	movzx  ecx,BYTE PTR [edi+eax*1]
  105500:	38 ca                	cmp    dl,cl
  105502:	75 14                	jne    105518 <memcmp+0x48>
    for (size_t i = 0; i < n; i++)
  105504:	83 c0 01             	add    eax,0x1
  105507:	39 c3                	cmp    ebx,eax
  105509:	75 ed                	jne    1054f8 <memcmp+0x28>
            return *(s1 + i) - *(s2 + i);
        }
    }

    return 0;
}
  10550b:	5b                   	pop    ebx
    return 0;
  10550c:	31 c0                	xor    eax,eax
}
  10550e:	5e                   	pop    esi
  10550f:	5f                   	pop    edi
  105510:	c3                   	ret    
  105511:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
            return *(s1 + i) - *(s2 + i);
  105518:	0f b6 c2             	movzx  eax,dl
}
  10551b:	5b                   	pop    ebx
            return *(s1 + i) - *(s2 + i);
  10551c:	29 c8                	sub    eax,ecx
}
  10551e:	5e                   	pop    esi
  10551f:	5f                   	pop    edi
  105520:	c3                   	ret    
  105521:	eb 0d                	jmp    105530 <memmove>
  105523:	90                   	nop
  105524:	90                   	nop
  105525:	90                   	nop
  105526:	90                   	nop
  105527:	90                   	nop
  105528:	90                   	nop
  105529:	90                   	nop
  10552a:	90                   	nop
  10552b:	90                   	nop
  10552c:	90                   	nop
  10552d:	90                   	nop
  10552e:	90                   	nop
  10552f:	90                   	nop

00105530 <memmove>:

void *memmove(void *dest, const void *src, size_t n)
{
  105530:	55                   	push   ebp
  105531:	57                   	push   edi
  105532:	56                   	push   esi
  105533:	53                   	push   ebx
  105534:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  105538:	8b 6c 24 18          	mov    ebp,DWORD PTR [esp+0x18]
  10553c:	8b 54 24 1c          	mov    edx,DWORD PTR [esp+0x1c]
    size_t i;
    const unsigned char *usrc = src;
    unsigned char *udest = dest;

    if (udest < usrc)
  105540:	39 c5                	cmp    ebp,eax
  105542:	0f 86 80 00 00 00    	jbe    1055c8 <memmove+0x98>
    {
        for (i = 0; i < n; i++)
  105548:	85 d2                	test   edx,edx
  10554a:	74 73                	je     1055bf <memmove+0x8f>
  10554c:	8d 48 04             	lea    ecx,[eax+0x4]
  10554f:	8d 75 04             	lea    esi,[ebp+0x4]
  105552:	39 cd                	cmp    ebp,ecx
  105554:	0f 93 c3             	setae  bl
  105557:	39 f0                	cmp    eax,esi
  105559:	0f 93 c1             	setae  cl
  10555c:	89 df                	mov    edi,ebx
  10555e:	89 ce                	mov    esi,ecx
  105560:	09 f7                	or     edi,esi
  105562:	83 fa 09             	cmp    edx,0x9
  105565:	0f 97 c1             	seta   cl
  105568:	89 fb                	mov    ebx,edi
  10556a:	84 cb                	test   bl,cl
  10556c:	74 7a                	je     1055e8 <memmove+0xb8>
  10556e:	89 c1                	mov    ecx,eax
  105570:	09 e9                	or     ecx,ebp
  105572:	83 e1 03             	and    ecx,0x3
  105575:	75 71                	jne    1055e8 <memmove+0xb8>
  105577:	89 d6                	mov    esi,edx
  105579:	31 c9                	xor    ecx,ecx
  10557b:	c1 ee 02             	shr    esi,0x2
  10557e:	66 90                	xchg   ax,ax
            udest[i] = usrc[i];
  105580:	8b 7c 8d 00          	mov    edi,DWORD PTR [ebp+ecx*4+0x0]
  105584:	89 3c 88             	mov    DWORD PTR [eax+ecx*4],edi
  105587:	83 c1 01             	add    ecx,0x1
  10558a:	39 ce                	cmp    esi,ecx
  10558c:	77 f2                	ja     105580 <memmove+0x50>
  10558e:	89 d6                	mov    esi,edx
  105590:	83 e6 fc             	and    esi,0xfffffffc
  105593:	39 f2                	cmp    edx,esi
  105595:	74 28                	je     1055bf <memmove+0x8f>
  105597:	0f b6 4c 35 00       	movzx  ecx,BYTE PTR [ebp+esi*1+0x0]
  10559c:	88 0c 30             	mov    BYTE PTR [eax+esi*1],cl
        for (i = 0; i < n; i++)
  10559f:	8d 4e 01             	lea    ecx,[esi+0x1]
  1055a2:	39 ca                	cmp    edx,ecx
  1055a4:	76 19                	jbe    1055bf <memmove+0x8f>
            udest[i] = usrc[i];
  1055a6:	0f b6 4c 35 01       	movzx  ecx,BYTE PTR [ebp+esi*1+0x1]
  1055ab:	88 4c 30 01          	mov    BYTE PTR [eax+esi*1+0x1],cl
        for (i = 0; i < n; i++)
  1055af:	8d 4e 02             	lea    ecx,[esi+0x2]
  1055b2:	39 ca                	cmp    edx,ecx
  1055b4:	76 09                	jbe    1055bf <memmove+0x8f>
            udest[i] = usrc[i];
  1055b6:	0f b6 54 35 02       	movzx  edx,BYTE PTR [ebp+esi*1+0x2]
  1055bb:	88 54 30 02          	mov    BYTE PTR [eax+esi*1+0x2],dl
        for (i = n; i > 0; i--)
            udest[i - 1] = usrc[i - 1];
    }

    return dest;
}
  1055bf:	5b                   	pop    ebx
  1055c0:	5e                   	pop    esi
  1055c1:	5f                   	pop    edi
  1055c2:	5d                   	pop    ebp
  1055c3:	c3                   	ret    
  1055c4:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
    else if (udest > usrc)
  1055c8:	73 f5                	jae    1055bf <memmove+0x8f>
        for (i = n; i > 0; i--)
  1055ca:	85 d2                	test   edx,edx
  1055cc:	74 f1                	je     1055bf <memmove+0x8f>
  1055ce:	66 90                	xchg   ax,ax
            udest[i - 1] = usrc[i - 1];
  1055d0:	83 ea 01             	sub    edx,0x1
  1055d3:	0f b6 4c 15 00       	movzx  ecx,BYTE PTR [ebp+edx*1+0x0]
        for (i = n; i > 0; i--)
  1055d8:	85 d2                	test   edx,edx
            udest[i - 1] = usrc[i - 1];
  1055da:	88 0c 10             	mov    BYTE PTR [eax+edx*1],cl
        for (i = n; i > 0; i--)
  1055dd:	75 f1                	jne    1055d0 <memmove+0xa0>
}
  1055df:	5b                   	pop    ebx
  1055e0:	5e                   	pop    esi
  1055e1:	5f                   	pop    edi
  1055e2:	5d                   	pop    ebp
  1055e3:	c3                   	ret    
  1055e4:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
        for (i = 0; i < n; i++)
  1055e8:	31 f6                	xor    esi,esi
  1055ea:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
            udest[i] = usrc[i];
  1055f0:	0f b6 4c 35 00       	movzx  ecx,BYTE PTR [ebp+esi*1+0x0]
  1055f5:	88 0c 30             	mov    BYTE PTR [eax+esi*1],cl
        for (i = 0; i < n; i++)
  1055f8:	83 c6 01             	add    esi,0x1
  1055fb:	39 f2                	cmp    edx,esi
  1055fd:	75 f1                	jne    1055f0 <memmove+0xc0>
  1055ff:	eb be                	jmp    1055bf <memmove+0x8f>
  105601:	eb 0d                	jmp    105610 <memcpy>
  105603:	90                   	nop
  105604:	90                   	nop
  105605:	90                   	nop
  105606:	90                   	nop
  105607:	90                   	nop
  105608:	90                   	nop
  105609:	90                   	nop
  10560a:	90                   	nop
  10560b:	90                   	nop
  10560c:	90                   	nop
  10560d:	90                   	nop
  10560e:	90                   	nop
  10560f:	90                   	nop

00105610 <memcpy>:

void *memcpy(void *dest, const void *src, size_t n)
{
  105610:	55                   	push   ebp
  105611:	57                   	push   edi
  105612:	56                   	push   esi
  105613:	53                   	push   ebx
  105614:	8b 6c 24 1c          	mov    ebp,DWORD PTR [esp+0x1c]
  105618:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  10561c:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
    char *d = dest;
    const char *s = src;

    for (size_t i = 0; i < n; i++)
  105620:	85 ed                	test   ebp,ebp
  105622:	74 79                	je     10569d <memcpy+0x8d>
  105624:	8d 50 04             	lea    edx,[eax+0x4]
  105627:	8d 71 04             	lea    esi,[ecx+0x4]
  10562a:	39 d1                	cmp    ecx,edx
  10562c:	0f 93 c3             	setae  bl
  10562f:	39 f0                	cmp    eax,esi
  105631:	0f 93 c2             	setae  dl
  105634:	89 df                	mov    edi,ebx
  105636:	89 d6                	mov    esi,edx
  105638:	09 f7                	or     edi,esi
  10563a:	83 fd 09             	cmp    ebp,0x9
  10563d:	0f 97 c2             	seta   dl
  105640:	89 fb                	mov    ebx,edi
  105642:	84 d3                	test   bl,dl
  105644:	74 62                	je     1056a8 <memcpy+0x98>
  105646:	89 ca                	mov    edx,ecx
  105648:	09 c2                	or     edx,eax
  10564a:	83 e2 03             	and    edx,0x3
  10564d:	75 59                	jne    1056a8 <memcpy+0x98>
  10564f:	89 ee                	mov    esi,ebp
  105651:	31 d2                	xor    edx,edx
  105653:	c1 ee 02             	shr    esi,0x2
  105656:	8d 76 00             	lea    esi,[esi+0x0]
  105659:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
    {
        d[i] = s[i];
  105660:	8b 3c 91             	mov    edi,DWORD PTR [ecx+edx*4]
  105663:	89 3c 90             	mov    DWORD PTR [eax+edx*4],edi
  105666:	83 c2 01             	add    edx,0x1
  105669:	39 d6                	cmp    esi,edx
  10566b:	77 f3                	ja     105660 <memcpy+0x50>
  10566d:	89 ee                	mov    esi,ebp
  10566f:	83 e6 fc             	and    esi,0xfffffffc
  105672:	39 f5                	cmp    ebp,esi
  105674:	74 27                	je     10569d <memcpy+0x8d>
  105676:	0f b6 14 31          	movzx  edx,BYTE PTR [ecx+esi*1]
  10567a:	88 14 30             	mov    BYTE PTR [eax+esi*1],dl
    for (size_t i = 0; i < n; i++)
  10567d:	8d 56 01             	lea    edx,[esi+0x1]
  105680:	39 d5                	cmp    ebp,edx
  105682:	76 19                	jbe    10569d <memcpy+0x8d>
        d[i] = s[i];
  105684:	0f b6 54 31 01       	movzx  edx,BYTE PTR [ecx+esi*1+0x1]
  105689:	88 54 30 01          	mov    BYTE PTR [eax+esi*1+0x1],dl
    for (size_t i = 0; i < n; i++)
  10568d:	8d 56 02             	lea    edx,[esi+0x2]
  105690:	39 d5                	cmp    ebp,edx
  105692:	76 09                	jbe    10569d <memcpy+0x8d>
        d[i] = s[i];
  105694:	0f b6 54 31 02       	movzx  edx,BYTE PTR [ecx+esi*1+0x2]
  105699:	88 54 30 02          	mov    BYTE PTR [eax+esi*1+0x2],dl
    }

    return dest;
}
  10569d:	5b                   	pop    ebx
  10569e:	5e                   	pop    esi
  10569f:	5f                   	pop    edi
  1056a0:	5d                   	pop    ebp
  1056a1:	c3                   	ret    
  1056a2:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
    for (size_t i = 0; i < n; i++)
  1056a8:	31 f6                	xor    esi,esi
  1056aa:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
        d[i] = s[i];
  1056b0:	0f b6 14 31          	movzx  edx,BYTE PTR [ecx+esi*1]
  1056b4:	88 14 30             	mov    BYTE PTR [eax+esi*1],dl
    for (size_t i = 0; i < n; i++)
  1056b7:	83 c6 01             	add    esi,0x1
  1056ba:	39 f5                	cmp    ebp,esi
  1056bc:	75 f2                	jne    1056b0 <memcpy+0xa0>
}
  1056be:	5b                   	pop    ebx
  1056bf:	5e                   	pop    esi
  1056c0:	5f                   	pop    edi
  1056c1:	5d                   	pop    ebp
  1056c2:	c3                   	ret    
  1056c3:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
  1056c9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

001056d0 <memset>:

void *memset(void *str, int c, size_t n)
{
  1056d0:	55                   	push   ebp
  1056d1:	57                   	push   edi
  1056d2:	56                   	push   esi
  1056d3:	53                   	push   ebx
  1056d4:	83 ec 10             	sub    esp,0x10
  1056d7:	8b 6c 24 2c          	mov    ebp,DWORD PTR [esp+0x2c]
  1056db:	8b 44 24 24          	mov    eax,DWORD PTR [esp+0x24]
  1056df:	8b 5c 24 28          	mov    ebx,DWORD PTR [esp+0x28]
    char *s = str;

    for (size_t i = 0; i < n; i++)
  1056e3:	85 ed                	test   ebp,ebp
  1056e5:	0f 84 e3 00 00 00    	je     1057ce <memset+0xfe>
  1056eb:	89 c2                	mov    edx,eax
  1056ed:	8d 75 ff             	lea    esi,[ebp-0x1]
  1056f0:	bf 05 00 00 00       	mov    edi,0x5
  1056f5:	f7 da                	neg    edx
  1056f7:	88 5c 24 07          	mov    BYTE PTR [esp+0x7],bl
  1056fb:	83 e2 03             	and    edx,0x3
  1056fe:	8d 4a 03             	lea    ecx,[edx+0x3]
  105701:	83 f9 05             	cmp    ecx,0x5
  105704:	0f 42 cf             	cmovb  ecx,edi
  105707:	39 ce                	cmp    esi,ecx
  105709:	0f 82 d1 00 00 00    	jb     1057e0 <memset+0x110>
  10570f:	85 d2                	test   edx,edx
  105711:	c7 04 24 00 00 00 00 	mov    DWORD PTR [esp],0x0
  105718:	74 27                	je     105741 <memset+0x71>
  10571a:	83 fa 01             	cmp    edx,0x1
    {
        *(s + i) = c;
  10571d:	88 18                	mov    BYTE PTR [eax],bl
    for (size_t i = 0; i < n; i++)
  10571f:	c7 04 24 01 00 00 00 	mov    DWORD PTR [esp],0x1
  105726:	74 19                	je     105741 <memset+0x71>
  105728:	83 fa 03             	cmp    edx,0x3
        *(s + i) = c;
  10572b:	88 58 01             	mov    BYTE PTR [eax+0x1],bl
    for (size_t i = 0; i < n; i++)
  10572e:	c7 04 24 02 00 00 00 	mov    DWORD PTR [esp],0x2
  105735:	75 0a                	jne    105741 <memset+0x71>
        *(s + i) = c;
  105737:	88 58 02             	mov    BYTE PTR [eax+0x2],bl
    for (size_t i = 0; i < n; i++)
  10573a:	c7 04 24 03 00 00 00 	mov    DWORD PTR [esp],0x3
  105741:	89 ee                	mov    esi,ebp
  105743:	31 c9                	xor    ecx,ecx
  105745:	8a 4c 24 07          	mov    cl,BYTE PTR [esp+0x7]
  105749:	29 d6                	sub    esi,edx
  10574b:	0f b6 fb             	movzx  edi,bl
  10574e:	89 74 24 08          	mov    DWORD PTR [esp+0x8],esi
  105752:	c1 ee 02             	shr    esi,0x2
  105755:	89 74 24 0c          	mov    DWORD PTR [esp+0xc],esi
  105759:	88 dd                	mov    ch,bl
  10575b:	89 fe                	mov    esi,edi
  10575d:	c1 e6 10             	shl    esi,0x10
  105760:	0f b7 c9             	movzx  ecx,cx
  105763:	c1 e7 18             	shl    edi,0x18
  105766:	09 f1                	or     ecx,esi
  105768:	8b 74 24 0c          	mov    esi,DWORD PTR [esp+0xc]
  10576c:	09 cf                	or     edi,ecx
  10576e:	8d 0c 10             	lea    ecx,[eax+edx*1]
  105771:	31 d2                	xor    edx,edx
  105773:	90                   	nop
  105774:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
        *(s + i) = c;
  105778:	89 3c 91             	mov    DWORD PTR [ecx+edx*4],edi
  10577b:	83 c2 01             	add    edx,0x1
  10577e:	39 d6                	cmp    esi,edx
  105780:	77 f6                	ja     105778 <memset+0xa8>
  105782:	8b 7c 24 08          	mov    edi,DWORD PTR [esp+0x8]
  105786:	8b 14 24             	mov    edx,DWORD PTR [esp]
  105789:	89 f9                	mov    ecx,edi
  10578b:	83 e1 fc             	and    ecx,0xfffffffc
  10578e:	01 ca                	add    edx,ecx
  105790:	39 cf                	cmp    edi,ecx
  105792:	74 3a                	je     1057ce <memset+0xfe>
    for (size_t i = 0; i < n; i++)
  105794:	8d 4a 01             	lea    ecx,[edx+0x1]
        *(s + i) = c;
  105797:	88 1c 10             	mov    BYTE PTR [eax+edx*1],bl
    for (size_t i = 0; i < n; i++)
  10579a:	39 cd                	cmp    ebp,ecx
  10579c:	76 30                	jbe    1057ce <memset+0xfe>
  10579e:	8d 4a 02             	lea    ecx,[edx+0x2]
        *(s + i) = c;
  1057a1:	88 5c 10 01          	mov    BYTE PTR [eax+edx*1+0x1],bl
    for (size_t i = 0; i < n; i++)
  1057a5:	39 cd                	cmp    ebp,ecx
  1057a7:	76 25                	jbe    1057ce <memset+0xfe>
  1057a9:	8d 4a 03             	lea    ecx,[edx+0x3]
        *(s + i) = c;
  1057ac:	88 5c 10 02          	mov    BYTE PTR [eax+edx*1+0x2],bl
    for (size_t i = 0; i < n; i++)
  1057b0:	39 cd                	cmp    ebp,ecx
  1057b2:	76 1a                	jbe    1057ce <memset+0xfe>
  1057b4:	8d 4a 04             	lea    ecx,[edx+0x4]
        *(s + i) = c;
  1057b7:	88 5c 10 03          	mov    BYTE PTR [eax+edx*1+0x3],bl
    for (size_t i = 0; i < n; i++)
  1057bb:	39 cd                	cmp    ebp,ecx
  1057bd:	76 0f                	jbe    1057ce <memset+0xfe>
  1057bf:	8d 4a 05             	lea    ecx,[edx+0x5]
        *(s + i) = c;
  1057c2:	88 5c 10 04          	mov    BYTE PTR [eax+edx*1+0x4],bl
    for (size_t i = 0; i < n; i++)
  1057c6:	39 cd                	cmp    ebp,ecx
  1057c8:	76 04                	jbe    1057ce <memset+0xfe>
        *(s + i) = c;
  1057ca:	88 5c 10 05          	mov    BYTE PTR [eax+edx*1+0x5],bl
    }

    return str;
}
  1057ce:	83 c4 10             	add    esp,0x10
  1057d1:	5b                   	pop    ebx
  1057d2:	5e                   	pop    esi
  1057d3:	5f                   	pop    edi
  1057d4:	5d                   	pop    ebp
  1057d5:	c3                   	ret    
  1057d6:	8d 76 00             	lea    esi,[esi+0x0]
  1057d9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
    for (size_t i = 0; i < n; i++)
  1057e0:	31 d2                	xor    edx,edx
  1057e2:	eb b0                	jmp    105794 <memset+0xc4>
  1057e4:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
  1057ea:	8d bf 00 00 00 00    	lea    edi,[edi+0x0]

001057f0 <memshift>:

void *memshift(char *mem, int shift, size_t n)
{
  1057f0:	55                   	push   ebp
  1057f1:	57                   	push   edi
  1057f2:	56                   	push   esi
  1057f3:	53                   	push   ebx
  1057f4:	8b 6c 24 14          	mov    ebp,DWORD PTR [esp+0x14]
  1057f8:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
  1057fc:	8b 54 24 1c          	mov    edx,DWORD PTR [esp+0x1c]
    void *dest = mem + shift;
  105800:	8d 44 0d 00          	lea    eax,[ebp+ecx*1+0x0]
    if (udest < usrc)
  105804:	39 c5                	cmp    ebp,eax
  105806:	0f 86 8c 00 00 00    	jbe    105898 <memshift+0xa8>
        for (i = 0; i < n; i++)
  10580c:	85 d2                	test   edx,edx
  10580e:	74 7f                	je     10588f <memshift+0x9f>
  105810:	83 c1 04             	add    ecx,0x4
  105813:	8d 75 04             	lea    esi,[ebp+0x4]
  105816:	85 c9                	test   ecx,ecx
  105818:	0f 9e c3             	setle  bl
  10581b:	39 f0                	cmp    eax,esi
  10581d:	0f 93 c1             	setae  cl
  105820:	89 df                	mov    edi,ebx
  105822:	89 ce                	mov    esi,ecx
  105824:	09 f7                	or     edi,esi
  105826:	83 fa 09             	cmp    edx,0x9
  105829:	0f 97 c1             	seta   cl
  10582c:	89 fb                	mov    ebx,edi
  10582e:	84 cb                	test   bl,cl
  105830:	0f 84 82 00 00 00    	je     1058b8 <memshift+0xc8>
  105836:	89 c1                	mov    ecx,eax
  105838:	09 e9                	or     ecx,ebp
  10583a:	83 e1 03             	and    ecx,0x3
  10583d:	75 79                	jne    1058b8 <memshift+0xc8>
  10583f:	89 d6                	mov    esi,edx
  105841:	31 c9                	xor    ecx,ecx
  105843:	c1 ee 02             	shr    esi,0x2
  105846:	8d 76 00             	lea    esi,[esi+0x0]
  105849:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
            udest[i] = usrc[i];
  105850:	8b 7c 8d 00          	mov    edi,DWORD PTR [ebp+ecx*4+0x0]
  105854:	89 3c 88             	mov    DWORD PTR [eax+ecx*4],edi
  105857:	83 c1 01             	add    ecx,0x1
  10585a:	39 ce                	cmp    esi,ecx
  10585c:	77 f2                	ja     105850 <memshift+0x60>
  10585e:	89 d6                	mov    esi,edx
  105860:	83 e6 fc             	and    esi,0xfffffffc
  105863:	39 f2                	cmp    edx,esi
  105865:	74 28                	je     10588f <memshift+0x9f>
  105867:	0f b6 4c 35 00       	movzx  ecx,BYTE PTR [ebp+esi*1+0x0]
  10586c:	88 0c 30             	mov    BYTE PTR [eax+esi*1],cl
        for (i = 0; i < n; i++)
  10586f:	8d 4e 01             	lea    ecx,[esi+0x1]
  105872:	39 ca                	cmp    edx,ecx
  105874:	76 19                	jbe    10588f <memshift+0x9f>
            udest[i] = usrc[i];
  105876:	0f b6 4c 35 01       	movzx  ecx,BYTE PTR [ebp+esi*1+0x1]
  10587b:	88 4c 30 01          	mov    BYTE PTR [eax+esi*1+0x1],cl
        for (i = 0; i < n; i++)
  10587f:	8d 4e 02             	lea    ecx,[esi+0x2]
  105882:	39 ca                	cmp    edx,ecx
  105884:	76 09                	jbe    10588f <memshift+0x9f>
            udest[i] = usrc[i];
  105886:	0f b6 54 35 02       	movzx  edx,BYTE PTR [ebp+esi*1+0x2]
  10588b:	88 54 30 02          	mov    BYTE PTR [eax+esi*1+0x2],dl
    memmove(dest, mem, n);
    return dest;
}
  10588f:	5b                   	pop    ebx
  105890:	5e                   	pop    esi
  105891:	5f                   	pop    edi
  105892:	5d                   	pop    ebp
  105893:	c3                   	ret    
  105894:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
    else if (udest > usrc)
  105898:	73 f5                	jae    10588f <memshift+0x9f>
        for (i = n; i > 0; i--)
  10589a:	85 d2                	test   edx,edx
  10589c:	74 f1                	je     10588f <memshift+0x9f>
  10589e:	66 90                	xchg   ax,ax
            udest[i - 1] = usrc[i - 1];
  1058a0:	83 ea 01             	sub    edx,0x1
  1058a3:	0f b6 4c 15 00       	movzx  ecx,BYTE PTR [ebp+edx*1+0x0]
        for (i = n; i > 0; i--)
  1058a8:	85 d2                	test   edx,edx
            udest[i - 1] = usrc[i - 1];
  1058aa:	88 0c 10             	mov    BYTE PTR [eax+edx*1],cl
        for (i = n; i > 0; i--)
  1058ad:	75 f1                	jne    1058a0 <memshift+0xb0>
}
  1058af:	5b                   	pop    ebx
  1058b0:	5e                   	pop    esi
  1058b1:	5f                   	pop    edi
  1058b2:	5d                   	pop    ebp
  1058b3:	c3                   	ret    
  1058b4:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
        for (i = 0; i < n; i++)
  1058b8:	31 f6                	xor    esi,esi
  1058ba:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
            udest[i] = usrc[i];
  1058c0:	0f b6 4c 35 00       	movzx  ecx,BYTE PTR [ebp+esi*1+0x0]
  1058c5:	88 0c 30             	mov    BYTE PTR [eax+esi*1],cl
        for (i = 0; i < n; i++)
  1058c8:	83 c6 01             	add    esi,0x1
  1058cb:	39 f2                	cmp    edx,esi
  1058cd:	75 f1                	jne    1058c0 <memshift+0xd0>
  1058cf:	eb be                	jmp    10588f <memshift+0x9f>
  1058d1:	eb 0d                	jmp    1058e0 <strcat>
  1058d3:	90                   	nop
  1058d4:	90                   	nop
  1058d5:	90                   	nop
  1058d6:	90                   	nop
  1058d7:	90                   	nop
  1058d8:	90                   	nop
  1058d9:	90                   	nop
  1058da:	90                   	nop
  1058db:	90                   	nop
  1058dc:	90                   	nop
  1058dd:	90                   	nop
  1058de:	90                   	nop
  1058df:	90                   	nop

001058e0 <strcat>:

char *strcat(char *dest, const char *src)
{
  1058e0:	55                   	push   ebp
  1058e1:	57                   	push   edi
  1058e2:	56                   	push   esi
  1058e3:	53                   	push   ebx
  1058e4:	83 ec 04             	sub    esp,0x4
  1058e7:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
  1058eb:	8b 4c 24 1c          	mov    ecx,DWORD PTR [esp+0x1c]
}

size_t strlen(const char *str)
{
    size_t lenght = 0;
    while (str[lenght])
  1058ef:	80 38 00             	cmp    BYTE PTR [eax],0x0
  1058f2:	0f 84 f0 00 00 00    	je     1059e8 <strcat+0x108>
  1058f8:	8d 50 01             	lea    edx,[eax+0x1]
    size_t lenght = 0;
  1058fb:	31 db                	xor    ebx,ebx
  1058fd:	8d 76 00             	lea    esi,[esi+0x0]
    while (str[lenght])
  105900:	89 d7                	mov    edi,edx
  105902:	83 c2 01             	add    edx,0x1
        lenght++;
  105905:	83 c3 01             	add    ebx,0x1
    while (str[lenght])
  105908:	80 7a ff 00          	cmp    BYTE PTR [edx-0x1],0x0
  10590c:	75 f2                	jne    105900 <strcat+0x20>
  10590e:	80 39 00             	cmp    BYTE PTR [ecx],0x0
  105911:	0f 84 9e 00 00 00    	je     1059b5 <strcat+0xd5>
    size_t lenght = 0;
  105917:	31 d2                	xor    edx,edx
  105919:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
        lenght++;
  105920:	83 c2 01             	add    edx,0x1
    while (str[lenght])
  105923:	80 3c 11 00          	cmp    BYTE PTR [ecx+edx*1],0x0
  105927:	75 f7                	jne    105920 <strcat+0x40>
  105929:	8d 3c 1a             	lea    edi,[edx+ebx*1]
  10592c:	8d 34 18             	lea    esi,[eax+ebx*1]
  10592f:	8d 5c 18 04          	lea    ebx,[eax+ebx*1+0x4]
  105933:	89 14 24             	mov    DWORD PTR [esp],edx
  105936:	01 c7                	add    edi,eax
  105938:	39 d9                	cmp    ecx,ebx
  10593a:	0f 93 c3             	setae  bl
  10593d:	89 dd                	mov    ebp,ebx
  10593f:	8d 59 04             	lea    ebx,[ecx+0x4]
  105942:	89 ea                	mov    edx,ebp
  105944:	89 cd                	mov    ebp,ecx
  105946:	39 de                	cmp    esi,ebx
  105948:	0f 93 c3             	setae  bl
  10594b:	09 f5                	or     ebp,esi
  10594d:	09 da                	or     edx,ebx
  10594f:	83 e5 03             	and    ebp,0x3
  105952:	0f 94 c3             	sete   bl
  105955:	84 da                	test   dl,bl
  105957:	74 67                	je     1059c0 <strcat+0xe0>
  105959:	8b 14 24             	mov    edx,DWORD PTR [esp]
  10595c:	83 fa 09             	cmp    edx,0x9
  10595f:	76 5f                	jbe    1059c0 <strcat+0xe0>
  105961:	89 d5                	mov    ebp,edx
  105963:	31 db                	xor    ebx,ebx
  105965:	89 44 24 18          	mov    DWORD PTR [esp+0x18],eax
  105969:	c1 ed 02             	shr    ebp,0x2
  10596c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
        d[dest_size + i] = src[i];
  105970:	8b 04 99             	mov    eax,DWORD PTR [ecx+ebx*4]
  105973:	89 04 9e             	mov    DWORD PTR [esi+ebx*4],eax
  105976:	83 c3 01             	add    ebx,0x1
  105979:	39 dd                	cmp    ebp,ebx
  10597b:	77 f3                	ja     105970 <strcat+0x90>
  10597d:	89 d5                	mov    ebp,edx
  10597f:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
  105983:	83 e5 fc             	and    ebp,0xfffffffc
  105986:	39 ea                	cmp    edx,ebp
  105988:	74 2b                	je     1059b5 <strcat+0xd5>
  10598a:	0f b6 1c 29          	movzx  ebx,BYTE PTR [ecx+ebp*1]
    for (size_t i = 0; i < src_size; i++)
  10598e:	89 14 24             	mov    DWORD PTR [esp],edx
        d[dest_size + i] = src[i];
  105991:	88 1c 2e             	mov    BYTE PTR [esi+ebp*1],bl
    for (size_t i = 0; i < src_size; i++)
  105994:	8d 5d 01             	lea    ebx,[ebp+0x1]
  105997:	39 da                	cmp    edx,ebx
  105999:	76 1a                	jbe    1059b5 <strcat+0xd5>
        d[dest_size + i] = src[i];
  10599b:	0f b6 54 29 01       	movzx  edx,BYTE PTR [ecx+ebp*1+0x1]
    for (size_t i = 0; i < src_size; i++)
  1059a0:	8d 5d 02             	lea    ebx,[ebp+0x2]
  1059a3:	39 1c 24             	cmp    DWORD PTR [esp],ebx
        d[dest_size + i] = src[i];
  1059a6:	88 54 2e 01          	mov    BYTE PTR [esi+ebp*1+0x1],dl
    for (size_t i = 0; i < src_size; i++)
  1059aa:	76 09                	jbe    1059b5 <strcat+0xd5>
        d[dest_size + i] = src[i];
  1059ac:	0f b6 54 29 02       	movzx  edx,BYTE PTR [ecx+ebp*1+0x2]
  1059b1:	88 54 2e 02          	mov    BYTE PTR [esi+ebp*1+0x2],dl
    d[dest_size + src_size] = '\0';
  1059b5:	c6 07 00             	mov    BYTE PTR [edi],0x0
}
  1059b8:	83 c4 04             	add    esp,0x4
  1059bb:	5b                   	pop    ebx
  1059bc:	5e                   	pop    esi
  1059bd:	5f                   	pop    edi
  1059be:	5d                   	pop    ebp
  1059bf:	c3                   	ret    
  1059c0:	8b 14 24             	mov    edx,DWORD PTR [esp]
  1059c3:	01 ca                	add    edx,ecx
  1059c5:	8d 76 00             	lea    esi,[esi+0x0]
        d[dest_size + i] = src[i];
  1059c8:	0f b6 19             	movzx  ebx,BYTE PTR [ecx]
  1059cb:	83 c1 01             	add    ecx,0x1
  1059ce:	83 c6 01             	add    esi,0x1
  1059d1:	88 5e ff             	mov    BYTE PTR [esi-0x1],bl
    for (size_t i = 0; i < src_size; i++)
  1059d4:	39 d1                	cmp    ecx,edx
  1059d6:	75 f0                	jne    1059c8 <strcat+0xe8>
    d[dest_size + src_size] = '\0';
  1059d8:	c6 07 00             	mov    BYTE PTR [edi],0x0
}
  1059db:	83 c4 04             	add    esp,0x4
  1059de:	5b                   	pop    ebx
  1059df:	5e                   	pop    esi
  1059e0:	5f                   	pop    edi
  1059e1:	5d                   	pop    ebp
  1059e2:	c3                   	ret    
  1059e3:	90                   	nop
  1059e4:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
    while (str[lenght])
  1059e8:	89 c7                	mov    edi,eax
    size_t lenght = 0;
  1059ea:	31 db                	xor    ebx,ebx
  1059ec:	e9 1d ff ff ff       	jmp    10590e <strcat+0x2e>
  1059f1:	eb 0d                	jmp    105a00 <strncat>
  1059f3:	90                   	nop
  1059f4:	90                   	nop
  1059f5:	90                   	nop
  1059f6:	90                   	nop
  1059f7:	90                   	nop
  1059f8:	90                   	nop
  1059f9:	90                   	nop
  1059fa:	90                   	nop
  1059fb:	90                   	nop
  1059fc:	90                   	nop
  1059fd:	90                   	nop
  1059fe:	90                   	nop
  1059ff:	90                   	nop

00105a00 <strncat>:
{
  105a00:	55                   	push   ebp
  105a01:	57                   	push   edi
  105a02:	56                   	push   esi
  105a03:	53                   	push   ebx
    while (str[lenght])
  105a04:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
{
  105a08:	8b 74 24 1c          	mov    esi,DWORD PTR [esp+0x1c]
    while (str[lenght])
  105a0c:	80 38 00             	cmp    BYTE PTR [eax],0x0
  105a0f:	0f 84 13 01 00 00    	je     105b28 <strncat+0x128>
  105a15:	8d 50 01             	lea    edx,[eax+0x1]
  105a18:	90                   	nop
  105a19:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
  105a20:	89 d3                	mov    ebx,edx
  105a22:	83 c2 01             	add    edx,0x1
  105a25:	80 7a ff 00          	cmp    BYTE PTR [edx-0x1],0x0
  105a29:	75 f5                	jne    105a20 <strncat+0x20>

size_t strnlen(const char *s, size_t maxlen)
{
    size_t len;

    for (len = 0; len < maxlen; len++, s++)
  105a2b:	85 f6                	test   esi,esi
  105a2d:	0f 84 fd 00 00 00    	je     105b30 <strncat+0x130>
    {
        if (!*s)
  105a33:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
    for (len = 0; len < maxlen; len++, s++)
  105a37:	31 d2                	xor    edx,edx
        if (!*s)
  105a39:	80 38 00             	cmp    BYTE PTR [eax],0x0
  105a3c:	75 14                	jne    105a52 <strncat+0x52>
  105a3e:	e9 ed 00 00 00       	jmp    105b30 <strncat+0x130>
  105a43:	90                   	nop
  105a44:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
  105a48:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
  105a4c:	80 3c 10 00          	cmp    BYTE PTR [eax+edx*1],0x0
  105a50:	74 07                	je     105a59 <strncat+0x59>
    for (len = 0; len < maxlen; len++, s++)
  105a52:	83 c2 01             	add    edx,0x1
  105a55:	39 d6                	cmp    esi,edx
  105a57:	75 ef                	jne    105a48 <strncat+0x48>
  105a59:	8d 73 04             	lea    esi,[ebx+0x4]
  105a5c:	39 74 24 18          	cmp    DWORD PTR [esp+0x18],esi
    s1[ss] = '\0';
  105a60:	c6 04 13 00          	mov    BYTE PTR [ebx+edx*1],0x0
  105a64:	0f 93 c0             	setae  al
  105a67:	89 c6                	mov    esi,eax
  105a69:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
  105a6d:	8d 78 04             	lea    edi,[eax+0x4]
  105a70:	39 fb                	cmp    ebx,edi
  105a72:	0f 93 c0             	setae  al
  105a75:	89 c7                	mov    edi,eax
  105a77:	09 fe                	or     esi,edi
  105a79:	8b 7c 24 18          	mov    edi,DWORD PTR [esp+0x18]
  105a7d:	09 df                	or     edi,ebx
  105a7f:	83 e7 03             	and    edi,0x3
  105a82:	0f 94 c0             	sete   al
  105a85:	89 c7                	mov    edi,eax
  105a87:	89 f0                	mov    eax,esi
  105a89:	89 f9                	mov    ecx,edi
  105a8b:	84 c8                	test   al,cl
  105a8d:	74 71                	je     105b00 <strncat+0x100>
  105a8f:	83 fa 09             	cmp    edx,0x9
  105a92:	89 d7                	mov    edi,edx
  105a94:	0f 97 c0             	seta   al
  105a97:	c1 ef 02             	shr    edi,0x2
  105a9a:	31 f6                	xor    esi,esi
  105a9c:	84 c0                	test   al,al
  105a9e:	74 60                	je     105b00 <strncat+0x100>
        d[i] = s[i];
  105aa0:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
  105aa4:	8b 04 b0             	mov    eax,DWORD PTR [eax+esi*4]
  105aa7:	89 04 b3             	mov    DWORD PTR [ebx+esi*4],eax
  105aaa:	83 c6 01             	add    esi,0x1
  105aad:	39 f7                	cmp    edi,esi
  105aaf:	77 ef                	ja     105aa0 <strncat+0xa0>
  105ab1:	89 d6                	mov    esi,edx
  105ab3:	83 e6 fc             	and    esi,0xfffffffc
  105ab6:	39 f2                	cmp    edx,esi
  105ab8:	74 33                	je     105aed <strncat+0xed>
  105aba:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
    for (size_t i = 0; i < n; i++)
  105abe:	8d 7e 01             	lea    edi,[esi+0x1]
  105ac1:	39 fa                	cmp    edx,edi
        d[i] = s[i];
  105ac3:	0f b6 04 30          	movzx  eax,BYTE PTR [eax+esi*1]
  105ac7:	88 04 33             	mov    BYTE PTR [ebx+esi*1],al
    for (size_t i = 0; i < n; i++)
  105aca:	76 21                	jbe    105aed <strncat+0xed>
        d[i] = s[i];
  105acc:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
    for (size_t i = 0; i < n; i++)
  105ad0:	8d 7e 02             	lea    edi,[esi+0x2]
  105ad3:	39 fa                	cmp    edx,edi
        d[i] = s[i];
  105ad5:	0f b6 44 30 01       	movzx  eax,BYTE PTR [eax+esi*1+0x1]
  105ada:	88 44 33 01          	mov    BYTE PTR [ebx+esi*1+0x1],al
    for (size_t i = 0; i < n; i++)
  105ade:	76 0d                	jbe    105aed <strncat+0xed>
        d[i] = s[i];
  105ae0:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
  105ae4:	0f b6 54 30 02       	movzx  edx,BYTE PTR [eax+esi*1+0x2]
  105ae9:	88 54 33 02          	mov    BYTE PTR [ebx+esi*1+0x2],dl
}
  105aed:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  105af1:	5b                   	pop    ebx
  105af2:	5e                   	pop    esi
  105af3:	5f                   	pop    edi
  105af4:	5d                   	pop    ebp
  105af5:	c3                   	ret    
  105af6:	8d 76 00             	lea    esi,[esi+0x0]
  105af9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
    for (size_t i = 0; i < n; i++)
  105b00:	31 f6                	xor    esi,esi
  105b02:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
        d[i] = s[i];
  105b08:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
  105b0c:	0f b6 04 30          	movzx  eax,BYTE PTR [eax+esi*1]
  105b10:	88 04 33             	mov    BYTE PTR [ebx+esi*1],al
    for (size_t i = 0; i < n; i++)
  105b13:	83 c6 01             	add    esi,0x1
  105b16:	39 f2                	cmp    edx,esi
  105b18:	75 ee                	jne    105b08 <strncat+0x108>
}
  105b1a:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  105b1e:	5b                   	pop    ebx
  105b1f:	5e                   	pop    esi
  105b20:	5f                   	pop    edi
  105b21:	5d                   	pop    ebp
  105b22:	c3                   	ret    
  105b23:	90                   	nop
  105b24:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
  105b28:	89 c3                	mov    ebx,eax
  105b2a:	e9 fc fe ff ff       	jmp    105a2b <strncat+0x2b>
  105b2f:	90                   	nop
  105b30:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
    s1[ss] = '\0';
  105b34:	c6 03 00             	mov    BYTE PTR [ebx],0x0
}
  105b37:	5b                   	pop    ebx
  105b38:	5e                   	pop    esi
  105b39:	5f                   	pop    edi
  105b3a:	5d                   	pop    ebp
  105b3b:	c3                   	ret    
  105b3c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]

00105b40 <strchr>:
{
  105b40:	53                   	push   ebx
  105b41:	8b 44 24 08          	mov    eax,DWORD PTR [esp+0x8]
  105b45:	8b 54 24 0c          	mov    edx,DWORD PTR [esp+0xc]
        if (*p == c)
  105b49:	0f b6 18             	movzx  ebx,BYTE PTR [eax]
  105b4c:	38 da                	cmp    dl,bl
  105b4e:	74 1e                	je     105b6e <strchr+0x2e>
        if (*p == '\0')
  105b50:	84 db                	test   bl,bl
  105b52:	89 d1                	mov    ecx,edx
  105b54:	75 0e                	jne    105b64 <strchr+0x24>
  105b56:	eb 18                	jmp    105b70 <strchr+0x30>
  105b58:	90                   	nop
  105b59:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
  105b60:	84 d2                	test   dl,dl
  105b62:	74 0c                	je     105b70 <strchr+0x30>
    for (;; ++p)
  105b64:	83 c0 01             	add    eax,0x1
        if (*p == c)
  105b67:	0f b6 10             	movzx  edx,BYTE PTR [eax]
  105b6a:	38 ca                	cmp    dl,cl
  105b6c:	75 f2                	jne    105b60 <strchr+0x20>
}
  105b6e:	5b                   	pop    ebx
  105b6f:	c3                   	ret    
            return (NULL);
  105b70:	31 c0                	xor    eax,eax
}
  105b72:	5b                   	pop    ebx
  105b73:	c3                   	ret    
  105b74:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
  105b7a:	8d bf 00 00 00 00    	lea    edi,[edi+0x0]

00105b80 <strcmp>:
{
  105b80:	56                   	push   esi
  105b81:	53                   	push   ebx
  105b82:	8b 5c 24 0c          	mov    ebx,DWORD PTR [esp+0xc]
  105b86:	8b 74 24 10          	mov    esi,DWORD PTR [esp+0x10]
    for (i = 0; stra[i] == strb[i]; i++)
  105b8a:	0f b6 13             	movzx  edx,BYTE PTR [ebx]
  105b8d:	0f be 0e             	movsx  ecx,BYTE PTR [esi]
  105b90:	38 d1                	cmp    cl,dl
  105b92:	75 1f                	jne    105bb3 <strcmp+0x33>
            return 0;
  105b94:	31 c0                	xor    eax,eax
        if (stra[i] == '\0')
  105b96:	84 c9                	test   cl,cl
  105b98:	75 0a                	jne    105ba4 <strcmp+0x24>
  105b9a:	eb 1c                	jmp    105bb8 <strcmp+0x38>
  105b9c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
  105ba0:	84 d2                	test   dl,dl
  105ba2:	74 1c                	je     105bc0 <strcmp+0x40>
    for (i = 0; stra[i] == strb[i]; i++)
  105ba4:	83 c0 01             	add    eax,0x1
  105ba7:	0f b6 14 03          	movzx  edx,BYTE PTR [ebx+eax*1]
  105bab:	0f be 0c 06          	movsx  ecx,BYTE PTR [esi+eax*1]
  105baf:	38 ca                	cmp    dl,cl
  105bb1:	74 ed                	je     105ba0 <strcmp+0x20>
    return stra[i] - strb[i];
  105bb3:	0f be c2             	movsx  eax,dl
  105bb6:	29 c8                	sub    eax,ecx
}
  105bb8:	5b                   	pop    ebx
  105bb9:	5e                   	pop    esi
  105bba:	c3                   	ret    
  105bbb:	90                   	nop
  105bbc:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
            return 0;
  105bc0:	31 c0                	xor    eax,eax
}
  105bc2:	5b                   	pop    ebx
  105bc3:	5e                   	pop    esi
  105bc4:	c3                   	ret    
  105bc5:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
  105bc9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00105bd0 <strncmp>:
{
  105bd0:	56                   	push   esi
  105bd1:	53                   	push   ebx
  105bd2:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  105bd6:	8b 74 24 14          	mov    esi,DWORD PTR [esp+0x14]
  105bda:	8b 4c 24 0c          	mov    ecx,DWORD PTR [esp+0xc]
  105bde:	01 c6                	add    esi,eax
    while (n-- > 0)
  105be0:	eb 1c                	jmp    105bfe <strncmp+0x2e>
  105be2:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
        u1 = (unsigned char)*s1++;
  105be8:	83 c1 01             	add    ecx,0x1
        u2 = (unsigned char)*s2++;
  105beb:	83 c0 01             	add    eax,0x1
        u1 = (unsigned char)*s1++;
  105bee:	0f b6 51 ff          	movzx  edx,BYTE PTR [ecx-0x1]
        u2 = (unsigned char)*s2++;
  105bf2:	0f b6 58 ff          	movzx  ebx,BYTE PTR [eax-0x1]
        if (u1 != u2)
  105bf6:	38 da                	cmp    dl,bl
  105bf8:	75 16                	jne    105c10 <strncmp+0x40>
        if (u1 == '\0')
  105bfa:	84 d2                	test   dl,dl
  105bfc:	74 04                	je     105c02 <strncmp+0x32>
    while (n-- > 0)
  105bfe:	39 f0                	cmp    eax,esi
  105c00:	75 e6                	jne    105be8 <strncmp+0x18>
            return 0;
  105c02:	31 c0                	xor    eax,eax
}
  105c04:	5b                   	pop    ebx
  105c05:	5e                   	pop    esi
  105c06:	c3                   	ret    
  105c07:	89 f6                	mov    esi,esi
  105c09:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
            return u1 - u2;
  105c10:	0f b6 c2             	movzx  eax,dl
  105c13:	29 d8                	sub    eax,ebx
}
  105c15:	5b                   	pop    ebx
  105c16:	5e                   	pop    esi
  105c17:	c3                   	ret    
  105c18:	90                   	nop
  105c19:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]

00105c20 <strcoll>:
{
  105c20:	55                   	push   ebp
  105c21:	89 e5                	mov    ebp,esp
  105c23:	57                   	push   edi
  105c24:	56                   	push   esi
  105c25:	53                   	push   ebx
  105c26:	83 ec 2c             	sub    esp,0x2c
  105c29:	8b 5d 08             	mov    ebx,DWORD PTR [ebp+0x8]
  105c2c:	8b 4d 0c             	mov    ecx,DWORD PTR [ebp+0xc]
    while (str[lenght])
  105c2f:	80 3b 00             	cmp    BYTE PTR [ebx],0x0
  105c32:	0f 84 88 01 00 00    	je     105dc0 <strcoll+0x1a0>
    size_t lenght = 0;
  105c38:	31 d2                	xor    edx,edx
  105c3a:	eb 06                	jmp    105c42 <strcoll+0x22>
  105c3c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
        lenght++;
  105c40:	89 c2                	mov    edx,eax
    while (str[lenght])
  105c42:	80 7c 13 01 00       	cmp    BYTE PTR [ebx+edx*1+0x1],0x0
        lenght++;
  105c47:	8d 42 01             	lea    eax,[edx+0x1]
    while (str[lenght])
  105c4a:	75 f4                	jne    105c40 <strcoll+0x20>
size_t strxfrm(char *dest, const char *src, size_t n)
{
    size_t len;

    len = strlen(src);
    (void)memcpy((void *)dest, (void *)src, min(n, len + 1));
  105c4c:	8d 7a 02             	lea    edi,[edx+0x2]
    char t1[1 + strxfrm(0, s1, 0)];
  105c4f:	83 c2 11             	add    edx,0x11
    size_t lenght = 0;
  105c52:	31 c0                	xor    eax,eax
    char t1[1 + strxfrm(0, s1, 0)];
  105c54:	83 e2 f0             	and    edx,0xfffffff0
  105c57:	29 d4                	sub    esp,edx
  105c59:	89 e6                	mov    esi,esp
  105c5b:	eb 05                	jmp    105c62 <strcoll+0x42>
  105c5d:	8d 76 00             	lea    esi,[esi+0x0]
        lenght++;
  105c60:	89 d0                	mov    eax,edx
    while (str[lenght])
  105c62:	80 7c 03 01 00       	cmp    BYTE PTR [ebx+eax*1+0x1],0x0
        lenght++;
  105c67:	8d 50 01             	lea    edx,[eax+0x1]
    while (str[lenght])
  105c6a:	75 f4                	jne    105c60 <strcoll+0x40>
  105c6c:	83 c0 02             	add    eax,0x2
  105c6f:	89 f2                	mov    edx,esi
  105c71:	39 f8                	cmp    eax,edi
  105c73:	0f 47 c7             	cmova  eax,edi
  105c76:	09 da                	or     edx,ebx
  105c78:	83 e2 03             	and    edx,0x3
  105c7b:	0f 85 6f 01 00 00    	jne    105df0 <strcoll+0x1d0>
  105c81:	83 f8 08             	cmp    eax,0x8
  105c84:	0f 86 66 01 00 00    	jbe    105df0 <strcoll+0x1d0>
  105c8a:	89 c7                	mov    edi,eax
  105c8c:	31 d2                	xor    edx,edx
  105c8e:	c1 ef 02             	shr    edi,0x2
  105c91:	89 7d d4             	mov    DWORD PTR [ebp-0x2c],edi
  105c94:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
        d[i] = s[i];
  105c98:	8b 3c 93             	mov    edi,DWORD PTR [ebx+edx*4]
  105c9b:	89 3c 96             	mov    DWORD PTR [esi+edx*4],edi
  105c9e:	83 c2 01             	add    edx,0x1
  105ca1:	39 55 d4             	cmp    DWORD PTR [ebp-0x2c],edx
  105ca4:	77 f2                	ja     105c98 <strcoll+0x78>
  105ca6:	89 c7                	mov    edi,eax
  105ca8:	83 e7 fc             	and    edi,0xfffffffc
  105cab:	39 f8                	cmp    eax,edi
  105cad:	74 27                	je     105cd6 <strcoll+0xb6>
  105caf:	0f b6 14 3b          	movzx  edx,BYTE PTR [ebx+edi*1]
  105cb3:	88 14 3e             	mov    BYTE PTR [esi+edi*1],dl
    for (size_t i = 0; i < n; i++)
  105cb6:	8d 57 01             	lea    edx,[edi+0x1]
  105cb9:	39 d0                	cmp    eax,edx
  105cbb:	76 19                	jbe    105cd6 <strcoll+0xb6>
        d[i] = s[i];
  105cbd:	0f b6 54 3b 01       	movzx  edx,BYTE PTR [ebx+edi*1+0x1]
  105cc2:	88 54 3e 01          	mov    BYTE PTR [esi+edi*1+0x1],dl
    for (size_t i = 0; i < n; i++)
  105cc6:	8d 57 02             	lea    edx,[edi+0x2]
  105cc9:	39 d0                	cmp    eax,edx
  105ccb:	76 09                	jbe    105cd6 <strcoll+0xb6>
        d[i] = s[i];
  105ccd:	0f b6 44 3b 02       	movzx  eax,BYTE PTR [ebx+edi*1+0x2]
  105cd2:	88 44 3e 02          	mov    BYTE PTR [esi+edi*1+0x2],al
    while (str[lenght])
  105cd6:	80 39 00             	cmp    BYTE PTR [ecx],0x0
  105cd9:	0f 84 f1 00 00 00    	je     105dd0 <strcoll+0x1b0>
    size_t lenght = 0;
  105cdf:	31 d2                	xor    edx,edx
  105ce1:	eb 07                	jmp    105cea <strcoll+0xca>
  105ce3:	90                   	nop
  105ce4:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
        lenght++;
  105ce8:	89 c2                	mov    edx,eax
    while (str[lenght])
  105cea:	80 7c 11 01 00       	cmp    BYTE PTR [ecx+edx*1+0x1],0x0
        lenght++;
  105cef:	8d 42 01             	lea    eax,[edx+0x1]
    while (str[lenght])
  105cf2:	75 f4                	jne    105ce8 <strcoll+0xc8>
    (void)memcpy((void *)dest, (void *)src, min(n, len + 1));
  105cf4:	8d 7a 02             	lea    edi,[edx+0x2]
    char t2[1 + strxfrm(0, s2, 0)];
  105cf7:	83 c2 11             	add    edx,0x11
    size_t lenght = 0;
  105cfa:	31 c0                	xor    eax,eax
    char t2[1 + strxfrm(0, s2, 0)];
  105cfc:	83 e2 f0             	and    edx,0xfffffff0
  105cff:	29 d4                	sub    esp,edx
  105d01:	89 e3                	mov    ebx,esp
  105d03:	eb 05                	jmp    105d0a <strcoll+0xea>
  105d05:	8d 76 00             	lea    esi,[esi+0x0]
        lenght++;
  105d08:	89 d0                	mov    eax,edx
    while (str[lenght])
  105d0a:	80 7c 01 01 00       	cmp    BYTE PTR [ecx+eax*1+0x1],0x0
        lenght++;
  105d0f:	8d 50 01             	lea    edx,[eax+0x1]
    while (str[lenght])
  105d12:	75 f4                	jne    105d08 <strcoll+0xe8>
  105d14:	83 c0 02             	add    eax,0x2
  105d17:	89 ca                	mov    edx,ecx
  105d19:	39 f8                	cmp    eax,edi
  105d1b:	0f 47 c7             	cmova  eax,edi
  105d1e:	09 da                	or     edx,ebx
  105d20:	83 e2 03             	and    edx,0x3
  105d23:	0f 85 b0 00 00 00    	jne    105dd9 <strcoll+0x1b9>
  105d29:	83 f8 08             	cmp    eax,0x8
  105d2c:	0f 86 a7 00 00 00    	jbe    105dd9 <strcoll+0x1b9>
  105d32:	89 c7                	mov    edi,eax
  105d34:	31 d2                	xor    edx,edx
  105d36:	c1 ef 02             	shr    edi,0x2
  105d39:	89 7d d4             	mov    DWORD PTR [ebp-0x2c],edi
  105d3c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
        d[i] = s[i];
  105d40:	8b 3c 91             	mov    edi,DWORD PTR [ecx+edx*4]
  105d43:	89 3c 93             	mov    DWORD PTR [ebx+edx*4],edi
  105d46:	83 c2 01             	add    edx,0x1
  105d49:	3b 55 d4             	cmp    edx,DWORD PTR [ebp-0x2c]
  105d4c:	72 f2                	jb     105d40 <strcoll+0x120>
  105d4e:	89 c7                	mov    edi,eax
  105d50:	83 e7 fc             	and    edi,0xfffffffc
  105d53:	39 c7                	cmp    edi,eax
  105d55:	74 27                	je     105d7e <strcoll+0x15e>
  105d57:	0f b6 14 39          	movzx  edx,BYTE PTR [ecx+edi*1]
  105d5b:	88 14 3b             	mov    BYTE PTR [ebx+edi*1],dl
    for (size_t i = 0; i < n; i++)
  105d5e:	8d 57 01             	lea    edx,[edi+0x1]
  105d61:	39 c2                	cmp    edx,eax
  105d63:	73 19                	jae    105d7e <strcoll+0x15e>
        d[i] = s[i];
  105d65:	0f b6 54 39 01       	movzx  edx,BYTE PTR [ecx+edi*1+0x1]
  105d6a:	88 54 3b 01          	mov    BYTE PTR [ebx+edi*1+0x1],dl
    for (size_t i = 0; i < n; i++)
  105d6e:	8d 57 02             	lea    edx,[edi+0x2]
  105d71:	39 c2                	cmp    edx,eax
  105d73:	73 09                	jae    105d7e <strcoll+0x15e>
        d[i] = s[i];
  105d75:	0f b6 44 39 02       	movzx  eax,BYTE PTR [ecx+edi*1+0x2]
  105d7a:	88 44 3b 02          	mov    BYTE PTR [ebx+edi*1+0x2],al
    for (i = 0; stra[i] == strb[i]; i++)
  105d7e:	0f b6 16             	movzx  edx,BYTE PTR [esi]
  105d81:	0f be 0b             	movsx  ecx,BYTE PTR [ebx]
  105d84:	38 ca                	cmp    dl,cl
  105d86:	75 1b                	jne    105da3 <strcoll+0x183>
            return 0;
  105d88:	31 c0                	xor    eax,eax
        if (stra[i] == '\0')
  105d8a:	84 d2                	test   dl,dl
  105d8c:	75 06                	jne    105d94 <strcoll+0x174>
  105d8e:	eb 18                	jmp    105da8 <strcoll+0x188>
  105d90:	84 d2                	test   dl,dl
  105d92:	74 1c                	je     105db0 <strcoll+0x190>
    for (i = 0; stra[i] == strb[i]; i++)
  105d94:	83 c0 01             	add    eax,0x1
  105d97:	0f b6 14 06          	movzx  edx,BYTE PTR [esi+eax*1]
  105d9b:	0f be 0c 03          	movsx  ecx,BYTE PTR [ebx+eax*1]
  105d9f:	38 ca                	cmp    dl,cl
  105da1:	74 ed                	je     105d90 <strcoll+0x170>
    return stra[i] - strb[i];
  105da3:	0f be c2             	movsx  eax,dl
  105da6:	29 c8                	sub    eax,ecx
}
  105da8:	8d 65 f4             	lea    esp,[ebp-0xc]
  105dab:	5b                   	pop    ebx
  105dac:	5e                   	pop    esi
  105dad:	5f                   	pop    edi
  105dae:	5d                   	pop    ebp
  105daf:	c3                   	ret    
  105db0:	8d 65 f4             	lea    esp,[ebp-0xc]
            return 0;
  105db3:	31 c0                	xor    eax,eax
}
  105db5:	5b                   	pop    ebx
  105db6:	5e                   	pop    esi
  105db7:	5f                   	pop    edi
  105db8:	5d                   	pop    ebp
  105db9:	c3                   	ret    
  105dba:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
    while (str[lenght])
  105dc0:	80 39 00             	cmp    BYTE PTR [ecx],0x0
        d[i] = s[i];
  105dc3:	c6 45 e6 00          	mov    BYTE PTR [ebp-0x1a],0x0
    char t1[1 + strxfrm(0, s1, 0)];
  105dc7:	8d 75 e6             	lea    esi,[ebp-0x1a]
    while (str[lenght])
  105dca:	0f 85 0f ff ff ff    	jne    105cdf <strcoll+0xbf>
        d[i] = s[i];
  105dd0:	c6 45 e7 00          	mov    BYTE PTR [ebp-0x19],0x0
    char t2[1 + strxfrm(0, s2, 0)];
  105dd4:	8d 5d e7             	lea    ebx,[ebp-0x19]
  105dd7:	eb a5                	jmp    105d7e <strcoll+0x15e>
    while (str[lenght])
  105dd9:	31 ff                	xor    edi,edi
  105ddb:	90                   	nop
  105ddc:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
        d[i] = s[i];
  105de0:	0f b6 14 39          	movzx  edx,BYTE PTR [ecx+edi*1]
  105de4:	88 14 3b             	mov    BYTE PTR [ebx+edi*1],dl
    for (size_t i = 0; i < n; i++)
  105de7:	83 c7 01             	add    edi,0x1
  105dea:	39 c7                	cmp    edi,eax
  105dec:	75 f2                	jne    105de0 <strcoll+0x1c0>
  105dee:	eb 8e                	jmp    105d7e <strcoll+0x15e>
    while (str[lenght])
  105df0:	31 ff                	xor    edi,edi
  105df2:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
        d[i] = s[i];
  105df8:	0f b6 14 3b          	movzx  edx,BYTE PTR [ebx+edi*1]
  105dfc:	88 14 3e             	mov    BYTE PTR [esi+edi*1],dl
    for (size_t i = 0; i < n; i++)
  105dff:	83 c7 01             	add    edi,0x1
  105e02:	39 f8                	cmp    eax,edi
  105e04:	75 f2                	jne    105df8 <strcoll+0x1d8>
  105e06:	e9 cb fe ff ff       	jmp    105cd6 <strcoll+0xb6>
  105e0b:	90                   	nop
  105e0c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]

00105e10 <strcpy>:
{
  105e10:	53                   	push   ebx
  105e11:	8b 44 24 08          	mov    eax,DWORD PTR [esp+0x8]
  105e15:	8b 4c 24 0c          	mov    ecx,DWORD PTR [esp+0xc]
    char *s = s1;
  105e19:	89 c2                	mov    edx,eax
  105e1b:	90                   	nop
  105e1c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
    while ((*s++ = *s2++) != 0)
  105e20:	83 c1 01             	add    ecx,0x1
  105e23:	0f b6 59 ff          	movzx  ebx,BYTE PTR [ecx-0x1]
  105e27:	83 c2 01             	add    edx,0x1
  105e2a:	84 db                	test   bl,bl
  105e2c:	88 5a ff             	mov    BYTE PTR [edx-0x1],bl
  105e2f:	75 ef                	jne    105e20 <strcpy+0x10>
}
  105e31:	5b                   	pop    ebx
  105e32:	c3                   	ret    
  105e33:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
  105e39:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00105e40 <strncpy>:
{
  105e40:	55                   	push   ebp
  105e41:	57                   	push   edi
  105e42:	56                   	push   esi
  105e43:	53                   	push   ebx
  105e44:	83 ec 0c             	sub    esp,0xc
  105e47:	8b 5c 24 28          	mov    ebx,DWORD PTR [esp+0x28]
  105e4b:	8b 44 24 20          	mov    eax,DWORD PTR [esp+0x20]
    for (len = 0; len < maxlen; len++, s++)
  105e4f:	85 db                	test   ebx,ebx
  105e51:	0f 84 ba 00 00 00    	je     105f11 <strncpy+0xd1>
        if (!*s)
  105e57:	8b 4c 24 24          	mov    ecx,DWORD PTR [esp+0x24]
    for (len = 0; len < maxlen; len++, s++)
  105e5b:	31 d2                	xor    edx,edx
        if (!*s)
  105e5d:	80 39 00             	cmp    BYTE PTR [ecx],0x0
  105e60:	75 18                	jne    105e7a <strncpy+0x3a>
  105e62:	e9 bf 00 00 00       	jmp    105f26 <strncpy+0xe6>
  105e67:	89 f6                	mov    esi,esi
  105e69:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
  105e70:	80 3c 11 00          	cmp    BYTE PTR [ecx+edx*1],0x0
  105e74:	0f 84 a6 00 00 00    	je     105f20 <strncpy+0xe0>
    for (len = 0; len < maxlen; len++, s++)
  105e7a:	83 c2 01             	add    edx,0x1
  105e7d:	39 d3                	cmp    ebx,edx
  105e7f:	75 ef                	jne    105e70 <strncpy+0x30>
  105e81:	89 4c 24 24          	mov    DWORD PTR [esp+0x24],ecx
  105e85:	8d 58 04             	lea    ebx,[eax+0x4]
  105e88:	39 5c 24 24          	cmp    DWORD PTR [esp+0x24],ebx
  105e8c:	0f 93 c1             	setae  cl
  105e8f:	89 cf                	mov    edi,ecx
  105e91:	8b 4c 24 24          	mov    ecx,DWORD PTR [esp+0x24]
  105e95:	8d 71 04             	lea    esi,[ecx+0x4]
  105e98:	39 f0                	cmp    eax,esi
  105e9a:	0f 93 c3             	setae  bl
  105e9d:	89 de                	mov    esi,ebx
  105e9f:	09 f7                	or     edi,esi
  105ea1:	89 c6                	mov    esi,eax
  105ea3:	09 ce                	or     esi,ecx
  105ea5:	89 f9                	mov    ecx,edi
  105ea7:	83 e6 03             	and    esi,0x3
  105eaa:	0f 94 c3             	sete   bl
  105ead:	84 d9                	test   cl,bl
  105eaf:	0f 84 73 01 00 00    	je     106028 <strncpy+0x1e8>
  105eb5:	83 fa 09             	cmp    edx,0x9
  105eb8:	0f 86 6a 01 00 00    	jbe    106028 <strncpy+0x1e8>
    for (size_t i = 0; i < n; i++)
  105ebe:	8b 4c 24 24          	mov    ecx,DWORD PTR [esp+0x24]
  105ec2:	89 d7                	mov    edi,edx
  105ec4:	31 db                	xor    ebx,ebx
  105ec6:	c1 ef 02             	shr    edi,0x2
  105ec9:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
        d[i] = s[i];
  105ed0:	8b 34 99             	mov    esi,DWORD PTR [ecx+ebx*4]
  105ed3:	89 34 98             	mov    DWORD PTR [eax+ebx*4],esi
  105ed6:	83 c3 01             	add    ebx,0x1
  105ed9:	39 df                	cmp    edi,ebx
  105edb:	77 f3                	ja     105ed0 <strncpy+0x90>
  105edd:	89 d6                	mov    esi,edx
  105edf:	89 4c 24 24          	mov    DWORD PTR [esp+0x24],ecx
  105ee3:	83 e6 fc             	and    esi,0xfffffffc
  105ee6:	39 d6                	cmp    esi,edx
  105ee8:	74 27                	je     105f11 <strncpy+0xd1>
  105eea:	0f b6 1c 31          	movzx  ebx,BYTE PTR [ecx+esi*1]
  105eee:	88 1c 30             	mov    BYTE PTR [eax+esi*1],bl
    for (size_t i = 0; i < n; i++)
  105ef1:	8d 5e 01             	lea    ebx,[esi+0x1]
  105ef4:	39 d3                	cmp    ebx,edx
  105ef6:	73 19                	jae    105f11 <strncpy+0xd1>
        d[i] = s[i];
  105ef8:	0f b6 5c 31 01       	movzx  ebx,BYTE PTR [ecx+esi*1+0x1]
  105efd:	88 5c 30 01          	mov    BYTE PTR [eax+esi*1+0x1],bl
    for (size_t i = 0; i < n; i++)
  105f01:	8d 5e 02             	lea    ebx,[esi+0x2]
  105f04:	39 d3                	cmp    ebx,edx
  105f06:	73 09                	jae    105f11 <strncpy+0xd1>
        d[i] = s[i];
  105f08:	0f b6 54 31 02       	movzx  edx,BYTE PTR [ecx+esi*1+0x2]
  105f0d:	88 54 30 02          	mov    BYTE PTR [eax+esi*1+0x2],dl
}
  105f11:	83 c4 0c             	add    esp,0xc
  105f14:	5b                   	pop    ebx
  105f15:	5e                   	pop    esi
  105f16:	5f                   	pop    edi
  105f17:	5d                   	pop    ebp
  105f18:	c3                   	ret    
  105f19:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
  105f20:	89 4c 24 24          	mov    DWORD PTR [esp+0x24],ecx
  105f24:	29 d3                	sub    ebx,edx
  105f26:	8d 3c 10             	lea    edi,[eax+edx*1]
  105f29:	8d 6b ff             	lea    ebp,[ebx-0x1]
  105f2c:	89 fe                	mov    esi,edi
  105f2e:	89 f9                	mov    ecx,edi
  105f30:	89 7c 24 08          	mov    DWORD PTR [esp+0x8],edi
  105f34:	f7 de                	neg    esi
  105f36:	89 2c 24             	mov    DWORD PTR [esp],ebp
  105f39:	bd 05 00 00 00       	mov    ebp,0x5
  105f3e:	83 e6 03             	and    esi,0x3
  105f41:	8d 7e 03             	lea    edi,[esi+0x3]
  105f44:	83 ff 05             	cmp    edi,0x5
  105f47:	0f 42 fd             	cmovb  edi,ebp
  105f4a:	39 3c 24             	cmp    DWORD PTR [esp],edi
  105f4d:	0f 82 f3 00 00 00    	jb     106046 <strncpy+0x206>
  105f53:	85 f6                	test   esi,esi
    for (size_t i = 0; i < n; i++)
  105f55:	c7 04 24 00 00 00 00 	mov    DWORD PTR [esp],0x0
  105f5c:	74 2c                	je     105f8a <strncpy+0x14a>
  105f5e:	83 fe 01             	cmp    esi,0x1
        *(s + i) = c;
  105f61:	c6 01 00             	mov    BYTE PTR [ecx],0x0
    for (size_t i = 0; i < n; i++)
  105f64:	c7 04 24 01 00 00 00 	mov    DWORD PTR [esp],0x1
  105f6b:	74 1d                	je     105f8a <strncpy+0x14a>
  105f6d:	83 fe 03             	cmp    esi,0x3
        *(s + i) = c;
  105f70:	c6 44 10 01 00       	mov    BYTE PTR [eax+edx*1+0x1],0x0
    for (size_t i = 0; i < n; i++)
  105f75:	c7 04 24 02 00 00 00 	mov    DWORD PTR [esp],0x2
  105f7c:	75 0c                	jne    105f8a <strncpy+0x14a>
        *(s + i) = c;
  105f7e:	c6 44 10 02 00       	mov    BYTE PTR [eax+edx*1+0x2],0x0
    for (size_t i = 0; i < n; i++)
  105f83:	c7 04 24 03 00 00 00 	mov    DWORD PTR [esp],0x3
  105f8a:	89 df                	mov    edi,ebx
  105f8c:	8b 4c 24 24          	mov    ecx,DWORD PTR [esp+0x24]
  105f90:	29 f7                	sub    edi,esi
  105f92:	01 d6                	add    esi,edx
  105f94:	89 fd                	mov    ebp,edi
  105f96:	89 7c 24 04          	mov    DWORD PTR [esp+0x4],edi
  105f9a:	01 c6                	add    esi,eax
  105f9c:	c1 ed 02             	shr    ebp,0x2
  105f9f:	31 ff                	xor    edi,edi
  105fa1:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
        *(s + i) = c;
  105fa8:	c7 04 be 00 00 00 00 	mov    DWORD PTR [esi+edi*4],0x0
  105faf:	83 c7 01             	add    edi,0x1
  105fb2:	39 fd                	cmp    ebp,edi
  105fb4:	77 f2                	ja     105fa8 <strncpy+0x168>
  105fb6:	8b 6c 24 04          	mov    ebp,DWORD PTR [esp+0x4]
  105fba:	8b 3c 24             	mov    edi,DWORD PTR [esp]
  105fbd:	89 4c 24 24          	mov    DWORD PTR [esp+0x24],ecx
  105fc1:	89 ee                	mov    esi,ebp
  105fc3:	83 e6 fc             	and    esi,0xfffffffc
  105fc6:	01 f7                	add    edi,esi
  105fc8:	39 f5                	cmp    ebp,esi
  105fca:	74 45                	je     106011 <strncpy+0x1d1>
  105fcc:	8b 6c 24 08          	mov    ebp,DWORD PTR [esp+0x8]
    for (size_t i = 0; i < n; i++)
  105fd0:	8d 77 01             	lea    esi,[edi+0x1]
  105fd3:	39 f3                	cmp    ebx,esi
        *(s + i) = c;
  105fd5:	c6 44 3d 00 00       	mov    BYTE PTR [ebp+edi*1+0x0],0x0
    for (size_t i = 0; i < n; i++)
  105fda:	76 35                	jbe    106011 <strncpy+0x1d1>
  105fdc:	8d 77 02             	lea    esi,[edi+0x2]
        *(s + i) = c;
  105fdf:	c6 44 2f 01 00       	mov    BYTE PTR [edi+ebp*1+0x1],0x0
    for (size_t i = 0; i < n; i++)
  105fe4:	39 f3                	cmp    ebx,esi
  105fe6:	76 29                	jbe    106011 <strncpy+0x1d1>
  105fe8:	8d 77 03             	lea    esi,[edi+0x3]
        *(s + i) = c;
  105feb:	c6 44 2f 02 00       	mov    BYTE PTR [edi+ebp*1+0x2],0x0
    for (size_t i = 0; i < n; i++)
  105ff0:	39 f3                	cmp    ebx,esi
  105ff2:	76 1d                	jbe    106011 <strncpy+0x1d1>
  105ff4:	8d 77 04             	lea    esi,[edi+0x4]
        *(s + i) = c;
  105ff7:	c6 44 2f 03 00       	mov    BYTE PTR [edi+ebp*1+0x3],0x0
    for (size_t i = 0; i < n; i++)
  105ffc:	39 f3                	cmp    ebx,esi
  105ffe:	76 11                	jbe    106011 <strncpy+0x1d1>
        *(s + i) = c;
  106000:	c6 44 2f 04 00       	mov    BYTE PTR [edi+ebp*1+0x4],0x0
    for (size_t i = 0; i < n; i++)
  106005:	83 c7 05             	add    edi,0x5
  106008:	39 fb                	cmp    ebx,edi
  10600a:	76 05                	jbe    106011 <strncpy+0x1d1>
        *(s + i) = c;
  10600c:	c6 44 3d 00 00       	mov    BYTE PTR [ebp+edi*1+0x0],0x0
    for (size_t i = 0; i < n; i++)
  106011:	85 d2                	test   edx,edx
  106013:	0f 85 6c fe ff ff    	jne    105e85 <strncpy+0x45>
}
  106019:	83 c4 0c             	add    esp,0xc
  10601c:	5b                   	pop    ebx
  10601d:	5e                   	pop    esi
  10601e:	5f                   	pop    edi
  10601f:	5d                   	pop    ebp
  106020:	c3                   	ret    
  106021:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
    for (size_t i = 0; i < n; i++)
  106028:	8b 4c 24 24          	mov    ecx,DWORD PTR [esp+0x24]
  10602c:	31 f6                	xor    esi,esi
  10602e:	66 90                	xchg   ax,ax
        d[i] = s[i];
  106030:	0f b6 1c 31          	movzx  ebx,BYTE PTR [ecx+esi*1]
  106034:	88 1c 30             	mov    BYTE PTR [eax+esi*1],bl
    for (size_t i = 0; i < n; i++)
  106037:	83 c6 01             	add    esi,0x1
  10603a:	39 d6                	cmp    esi,edx
  10603c:	75 f2                	jne    106030 <strncpy+0x1f0>
}
  10603e:	83 c4 0c             	add    esp,0xc
  106041:	5b                   	pop    ebx
  106042:	5e                   	pop    esi
  106043:	5f                   	pop    edi
  106044:	5d                   	pop    ebp
  106045:	c3                   	ret    
    for (size_t i = 0; i < n; i++)
  106046:	31 ff                	xor    edi,edi
  106048:	e9 7f ff ff ff       	jmp    105fcc <strncpy+0x18c>
  10604d:	8d 76 00             	lea    esi,[esi+0x0]

00106050 <strcspn>:
{
  106050:	55                   	push   ebp
  106051:	57                   	push   edi
  106052:	56                   	push   esi
  106053:	53                   	push   ebx
  106054:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  106058:	8b 7c 24 18          	mov    edi,DWORD PTR [esp+0x18]
    for (s = (char *)string, c = *s; c != 0; s++, c = *s)
  10605c:	0f b6 18             	movzx  ebx,BYTE PTR [eax]
  10605f:	84 db                	test   bl,bl
  106061:	74 44                	je     1060a7 <strcspn+0x57>
        for (p = (char *)chars; *p != 0; p++)
  106063:	0f b6 37             	movzx  esi,BYTE PTR [edi]
  106066:	89 c5                	mov    ebp,eax
  106068:	89 f2                	mov    edx,esi
  10606a:	84 d2                	test   dl,dl
  10606c:	74 20                	je     10608e <strcspn+0x3e>
            if (c == *p)
  10606e:	38 d3                	cmp    bl,dl
  106070:	74 2e                	je     1060a0 <strcspn+0x50>
  106072:	89 fa                	mov    edx,edi
  106074:	eb 0e                	jmp    106084 <strcspn+0x34>
  106076:	8d 76 00             	lea    esi,[esi+0x0]
  106079:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
  106080:	38 d9                	cmp    cl,bl
  106082:	74 1c                	je     1060a0 <strcspn+0x50>
        for (p = (char *)chars; *p != 0; p++)
  106084:	83 c2 01             	add    edx,0x1
  106087:	0f b6 0a             	movzx  ecx,BYTE PTR [edx]
  10608a:	84 c9                	test   cl,cl
  10608c:	75 f2                	jne    106080 <strcspn+0x30>
    for (s = (char *)string, c = *s; c != 0; s++, c = *s)
  10608e:	83 c0 01             	add    eax,0x1
  106091:	0f b6 18             	movzx  ebx,BYTE PTR [eax]
  106094:	84 db                	test   bl,bl
  106096:	75 d0                	jne    106068 <strcspn+0x18>
  106098:	90                   	nop
  106099:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
  1060a0:	29 e8                	sub    eax,ebp
}
  1060a2:	5b                   	pop    ebx
  1060a3:	5e                   	pop    esi
  1060a4:	5f                   	pop    edi
  1060a5:	5d                   	pop    ebp
  1060a6:	c3                   	ret    
    for (s = (char *)string, c = *s; c != 0; s++, c = *s)
  1060a7:	31 c0                	xor    eax,eax
    return s - string;
  1060a9:	eb f7                	jmp    1060a2 <strcspn+0x52>
  1060ab:	90                   	nop
  1060ac:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]

001060b0 <strerror>:
}
  1060b0:	b8 8b 24 11 00       	mov    eax,0x11248b
  1060b5:	c3                   	ret    
  1060b6:	8d 76 00             	lea    esi,[esi+0x0]
  1060b9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

001060c0 <strlen>:
{
  1060c0:	8b 54 24 04          	mov    edx,DWORD PTR [esp+0x4]
    size_t lenght = 0;
  1060c4:	31 c0                	xor    eax,eax
    while (str[lenght])
  1060c6:	80 3a 00             	cmp    BYTE PTR [edx],0x0
  1060c9:	74 15                	je     1060e0 <strlen+0x20>
  1060cb:	90                   	nop
  1060cc:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
        lenght++;
  1060d0:	83 c0 01             	add    eax,0x1
    while (str[lenght])
  1060d3:	80 3c 02 00          	cmp    BYTE PTR [edx+eax*1],0x0
  1060d7:	75 f7                	jne    1060d0 <strlen+0x10>
  1060d9:	f3 c3                	repz ret 
  1060db:	90                   	nop
  1060dc:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
}
  1060e0:	f3 c3                	repz ret 
  1060e2:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
  1060e9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

001060f0 <strnlen>:
{
  1060f0:	8b 54 24 08          	mov    edx,DWORD PTR [esp+0x8]
    for (len = 0; len < maxlen; len++, s++)
  1060f4:	31 c0                	xor    eax,eax
{
  1060f6:	8b 4c 24 04          	mov    ecx,DWORD PTR [esp+0x4]
    for (len = 0; len < maxlen; len++, s++)
  1060fa:	85 d2                	test   edx,edx
  1060fc:	74 2a                	je     106128 <strnlen+0x38>
        if (!*s)
  1060fe:	80 39 00             	cmp    BYTE PTR [ecx],0x0
  106101:	75 0b                	jne    10610e <strnlen+0x1e>
  106103:	eb 1b                	jmp    106120 <strnlen+0x30>
  106105:	8d 76 00             	lea    esi,[esi+0x0]
  106108:	80 3c 01 00          	cmp    BYTE PTR [ecx+eax*1],0x0
  10610c:	74 12                	je     106120 <strnlen+0x30>
    for (len = 0; len < maxlen; len++, s++)
  10610e:	83 c0 01             	add    eax,0x1
  106111:	39 c2                	cmp    edx,eax
  106113:	75 f3                	jne    106108 <strnlen+0x18>
  106115:	f3 c3                	repz ret 
  106117:	89 f6                	mov    esi,esi
  106119:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
}
  106120:	f3 c3                	repz ret 
  106122:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
  106128:	f3 c3                	repz ret 
  10612a:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]

00106130 <strpbrk>:
{
  106130:	57                   	push   edi
  106131:	56                   	push   esi
  106132:	53                   	push   ebx
  106133:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  106137:	8b 7c 24 14          	mov    edi,DWORD PTR [esp+0x14]
    for (s = (char *)string, c = *s; c != 0; s++, c = *s)
  10613b:	0f b6 18             	movzx  ebx,BYTE PTR [eax]
  10613e:	84 db                	test   bl,bl
  106140:	74 2e                	je     106170 <strpbrk+0x40>
        for (p = (char *)chars; *p != 0; p++)
  106142:	0f b6 37             	movzx  esi,BYTE PTR [edi]
  106145:	89 f2                	mov    edx,esi
  106147:	84 d2                	test   dl,dl
  106149:	74 1b                	je     106166 <strpbrk+0x36>
            if (c == *p)
  10614b:	38 d3                	cmp    bl,dl
  10614d:	74 23                	je     106172 <strpbrk+0x42>
  10614f:	89 fa                	mov    edx,edi
  106151:	eb 09                	jmp    10615c <strpbrk+0x2c>
  106153:	90                   	nop
  106154:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
  106158:	38 d9                	cmp    cl,bl
  10615a:	74 16                	je     106172 <strpbrk+0x42>
        for (p = (char *)chars; *p != 0; p++)
  10615c:	83 c2 01             	add    edx,0x1
  10615f:	0f b6 0a             	movzx  ecx,BYTE PTR [edx]
  106162:	84 c9                	test   cl,cl
  106164:	75 f2                	jne    106158 <strpbrk+0x28>
    for (s = (char *)string, c = *s; c != 0; s++, c = *s)
  106166:	83 c0 01             	add    eax,0x1
  106169:	0f b6 18             	movzx  ebx,BYTE PTR [eax]
  10616c:	84 db                	test   bl,bl
  10616e:	75 d5                	jne    106145 <strpbrk+0x15>
    return *s ? (char *)s : NULL;
  106170:	31 c0                	xor    eax,eax
}
  106172:	5b                   	pop    ebx
  106173:	5e                   	pop    esi
  106174:	5f                   	pop    edi
  106175:	c3                   	ret    
  106176:	8d 76 00             	lea    esi,[esi+0x0]
  106179:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00106180 <strrchr>:
{
  106180:	56                   	push   esi
  106181:	53                   	push   ebx
    char *rtnval = 0;
  106182:	31 c0                	xor    eax,eax
{
  106184:	8b 54 24 0c          	mov    edx,DWORD PTR [esp+0xc]
  106188:	8b 74 24 10          	mov    esi,DWORD PTR [esp+0x10]
  10618c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
        if (*s == c)
  106190:	0f be 1a             	movsx  ebx,BYTE PTR [edx]
  106193:	39 de                	cmp    esi,ebx
  106195:	0f 44 c2             	cmove  eax,edx
    } while (*s++);
  106198:	83 c2 01             	add    edx,0x1
  10619b:	84 db                	test   bl,bl
  10619d:	75 f1                	jne    106190 <strrchr+0x10>
}
  10619f:	5b                   	pop    ebx
  1061a0:	5e                   	pop    esi
  1061a1:	c3                   	ret    
  1061a2:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
  1061a9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

001061b0 <strspn>:
{
  1061b0:	55                   	push   ebp
  1061b1:	57                   	push   edi
    for (i = 0; p[i]; i++)
  1061b2:	31 c0                	xor    eax,eax
{
  1061b4:	56                   	push   esi
  1061b5:	53                   	push   ebx
  1061b6:	8b 6c 24 14          	mov    ebp,DWORD PTR [esp+0x14]
  1061ba:	8b 7c 24 18          	mov    edi,DWORD PTR [esp+0x18]
    for (i = 0; p[i]; i++)
  1061be:	0f b6 5d 00          	movzx  ebx,BYTE PTR [ebp+0x0]
  1061c2:	84 db                	test   bl,bl
  1061c4:	74 32                	je     1061f8 <strspn+0x48>
        for (j = 0; s[j]; j++)
  1061c6:	0f b6 07             	movzx  eax,BYTE PTR [edi]
    for (i = 0; p[i]; i++)
  1061c9:	31 f6                	xor    esi,esi
  1061cb:	89 7c 24 18          	mov    DWORD PTR [esp+0x18],edi
  1061cf:	90                   	nop
        for (j = 0; s[j]; j++)
  1061d0:	84 c0                	test   al,al
  1061d2:	89 f7                	mov    edi,esi
  1061d4:	74 20                	je     1061f6 <strspn+0x46>
            if (s[j] == p[i])
  1061d6:	38 d8                	cmp    al,bl
  1061d8:	74 26                	je     106200 <strspn+0x50>
  1061da:	8b 54 24 18          	mov    edx,DWORD PTR [esp+0x18]
  1061de:	83 c2 01             	add    edx,0x1
  1061e1:	eb 0c                	jmp    1061ef <strspn+0x3f>
  1061e3:	90                   	nop
  1061e4:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
  1061e8:	83 c2 01             	add    edx,0x1
  1061eb:	38 d9                	cmp    cl,bl
  1061ed:	74 11                	je     106200 <strspn+0x50>
        for (j = 0; s[j]; j++)
  1061ef:	0f b6 0a             	movzx  ecx,BYTE PTR [edx]
  1061f2:	84 c9                	test   cl,cl
  1061f4:	75 f2                	jne    1061e8 <strspn+0x38>
  1061f6:	89 f8                	mov    eax,edi
}
  1061f8:	5b                   	pop    ebx
  1061f9:	5e                   	pop    esi
  1061fa:	5f                   	pop    edi
  1061fb:	5d                   	pop    ebp
  1061fc:	c3                   	ret    
  1061fd:	8d 76 00             	lea    esi,[esi+0x0]
    for (i = 0; p[i]; i++)
  106200:	83 c6 01             	add    esi,0x1
  106203:	0f b6 5c 35 00       	movzx  ebx,BYTE PTR [ebp+esi*1+0x0]
  106208:	89 f7                	mov    edi,esi
  10620a:	84 db                	test   bl,bl
  10620c:	75 c2                	jne    1061d0 <strspn+0x20>
  10620e:	eb e6                	jmp    1061f6 <strspn+0x46>

00106210 <strstr>:
{
  106210:	57                   	push   edi
  106211:	56                   	push   esi
  106212:	53                   	push   ebx
  106213:	8b 74 24 14          	mov    esi,DWORD PTR [esp+0x14]
  106217:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
    while (str[lenght])
  10621b:	0f b6 3e             	movzx  edi,BYTE PTR [esi]
  10621e:	89 fb                	mov    ebx,edi
  106220:	84 db                	test   bl,bl
    size_t lenght = 0;
  106222:	bb 00 00 00 00       	mov    ebx,0x0
    while (str[lenght])
  106227:	74 26                	je     10624f <strstr+0x3f>
  106229:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
        lenght++;
  106230:	83 c3 01             	add    ebx,0x1
    while (str[lenght])
  106233:	80 3c 1e 00          	cmp    BYTE PTR [esi+ebx*1],0x0
  106237:	75 f7                	jne    106230 <strstr+0x20>
        if (*p == c)
  106239:	0f b6 10             	movzx  edx,BYTE PTR [eax]
  10623c:	89 f9                	mov    ecx,edi
  10623e:	38 d1                	cmp    cl,dl
  106240:	74 16                	je     106258 <strstr+0x48>
  106242:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
        if (*p == '\0')
  106248:	84 d2                	test   dl,dl
  10624a:	74 34                	je     106280 <strstr+0x70>
    for (;; ++p)
  10624c:	83 c0 01             	add    eax,0x1
        if (*p == c)
  10624f:	0f b6 10             	movzx  edx,BYTE PTR [eax]
  106252:	89 f9                	mov    ecx,edi
  106254:	38 d1                	cmp    cl,dl
  106256:	75 f0                	jne    106248 <strstr+0x38>
    size_t lenght = 0;
  106258:	31 d2                	xor    edx,edx
  10625a:	eb 14                	jmp    106270 <strstr+0x60>
  10625c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
        u1 = (unsigned char)*s1++;
  106260:	0f b6 0c 10          	movzx  ecx,BYTE PTR [eax+edx*1]
        if (u1 != u2)
  106264:	3a 0c 16             	cmp    cl,BYTE PTR [esi+edx*1]
  106267:	75 e3                	jne    10624c <strstr+0x3c>
  106269:	83 c2 01             	add    edx,0x1
        if (u1 == '\0')
  10626c:	84 c9                	test   cl,cl
  10626e:	74 04                	je     106274 <strstr+0x64>
    while (n-- > 0)
  106270:	39 da                	cmp    edx,ebx
  106272:	75 ec                	jne    106260 <strstr+0x50>
}
  106274:	5b                   	pop    ebx
  106275:	5e                   	pop    esi
  106276:	5f                   	pop    edi
  106277:	c3                   	ret    
  106278:	90                   	nop
  106279:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
  106280:	5b                   	pop    ebx
    return NULL;
  106281:	31 c0                	xor    eax,eax
}
  106283:	5e                   	pop    esi
  106284:	5f                   	pop    edi
  106285:	c3                   	ret    
  106286:	8d 76 00             	lea    esi,[esi+0x0]
  106289:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00106290 <strtok>:
{
  106290:	55                   	push   ebp
  106291:	57                   	push   edi
  106292:	56                   	push   esi
  106293:	53                   	push   ebx
  106294:	8b 5c 24 14          	mov    ebx,DWORD PTR [esp+0x14]
  106298:	8b 7c 24 1c          	mov    edi,DWORD PTR [esp+0x1c]
    if (s == 0)
  10629c:	85 db                	test   ebx,ebx
  10629e:	0f 84 83 00 00 00    	je     106327 <strtok+0x97>
        if ((ch = *s++) == '\0')
  1062a4:	0f b6 03             	movzx  eax,BYTE PTR [ebx]
  1062a7:	84 c0                	test   al,al
  1062a9:	74 2d                	je     1062d8 <strtok+0x48>
        if (*p == c)
  1062ab:	0f b6 37             	movzx  esi,BYTE PTR [edi]
  1062ae:	89 f2                	mov    edx,esi
  1062b0:	38 d0                	cmp    al,dl
  1062b2:	74 1a                	je     1062ce <strtok+0x3e>
        if (*p == '\0')
  1062b4:	84 d2                	test   dl,dl
  1062b6:	74 28                	je     1062e0 <strtok+0x50>
  1062b8:	89 fa                	mov    edx,edi
  1062ba:	eb 08                	jmp    1062c4 <strtok+0x34>
  1062bc:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
  1062c0:	84 c9                	test   cl,cl
  1062c2:	74 1c                	je     1062e0 <strtok+0x50>
    for (;; ++p)
  1062c4:	83 c2 01             	add    edx,0x1
        if (*p == c)
  1062c7:	0f b6 0a             	movzx  ecx,BYTE PTR [edx]
  1062ca:	38 c8                	cmp    al,cl
  1062cc:	75 f2                	jne    1062c0 <strtok+0x30>
        if ((ch = *s++) == '\0')
  1062ce:	83 c3 01             	add    ebx,0x1
  1062d1:	0f b6 03             	movzx  eax,BYTE PTR [ebx]
  1062d4:	84 c0                	test   al,al
  1062d6:	75 d3                	jne    1062ab <strtok+0x1b>
            return 0;
  1062d8:	31 c0                	xor    eax,eax
}
  1062da:	5b                   	pop    ebx
  1062db:	5e                   	pop    esi
  1062dc:	5f                   	pop    edi
  1062dd:	5d                   	pop    ebp
  1062de:	c3                   	ret    
  1062df:	90                   	nop
            if (c == *p)
  1062e0:	89 dd                	mov    ebp,ebx
        for (p = (char *)chars; *p != 0; p++)
  1062e2:	89 f2                	mov    edx,esi
  1062e4:	84 d2                	test   dl,dl
  1062e6:	74 16                	je     1062fe <strtok+0x6e>
            if (c == *p)
  1062e8:	38 c2                	cmp    dl,al
  1062ea:	74 24                	je     106310 <strtok+0x80>
  1062ec:	89 fa                	mov    edx,edi
  1062ee:	eb 04                	jmp    1062f4 <strtok+0x64>
  1062f0:	38 c1                	cmp    cl,al
  1062f2:	74 1c                	je     106310 <strtok+0x80>
        for (p = (char *)chars; *p != 0; p++)
  1062f4:	83 c2 01             	add    edx,0x1
  1062f7:	0f b6 0a             	movzx  ecx,BYTE PTR [edx]
  1062fa:	84 c9                	test   cl,cl
  1062fc:	75 f2                	jne    1062f0 <strtok+0x60>
    for (s = (char *)string, c = *s; c != 0; s++, c = *s)
  1062fe:	83 c5 01             	add    ebp,0x1
  106301:	0f b6 45 00          	movzx  eax,BYTE PTR [ebp+0x0]
  106305:	84 c0                	test   al,al
  106307:	75 d9                	jne    1062e2 <strtok+0x52>
  106309:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
    *state = s + strcspn(s, delim);
  106310:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
  106314:	89 28                	mov    DWORD PTR [eax],ebp
    if (**state != 0)
  106316:	80 7d 00 00          	cmp    BYTE PTR [ebp+0x0],0x0
  10631a:	89 d8                	mov    eax,ebx
  10631c:	74 bc                	je     1062da <strtok+0x4a>
        **state++ = 0;
  10631e:	c6 45 00 00          	mov    BYTE PTR [ebp+0x0],0x0
}
  106322:	5b                   	pop    ebx
  106323:	5e                   	pop    esi
  106324:	5f                   	pop    edi
  106325:	5d                   	pop    ebp
  106326:	c3                   	ret    
        s = *state;
  106327:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
  10632b:	8b 18                	mov    ebx,DWORD PTR [eax]
  10632d:	e9 72 ff ff ff       	jmp    1062a4 <strtok+0x14>
  106332:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
  106339:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00106340 <strxfrm>:
{
  106340:	55                   	push   ebp
  106341:	57                   	push   edi
  106342:	56                   	push   esi
  106343:	53                   	push   ebx
  106344:	83 ec 04             	sub    esp,0x4
  106347:	8b 4c 24 1c          	mov    ecx,DWORD PTR [esp+0x1c]
  10634b:	8b 6c 24 18          	mov    ebp,DWORD PTR [esp+0x18]
  10634f:	8b 74 24 20          	mov    esi,DWORD PTR [esp+0x20]
    while (str[lenght])
  106353:	80 39 00             	cmp    BYTE PTR [ecx],0x0
  106356:	0f 84 d4 00 00 00    	je     106430 <strxfrm+0xf0>
    size_t lenght = 0;
  10635c:	31 d2                	xor    edx,edx
  10635e:	eb 02                	jmp    106362 <strxfrm+0x22>
        lenght++;
  106360:	89 c2                	mov    edx,eax
    while (str[lenght])
  106362:	80 7c 11 01 00       	cmp    BYTE PTR [ecx+edx*1+0x1],0x0
        lenght++;
  106367:	8d 42 01             	lea    eax,[edx+0x1]
    while (str[lenght])
  10636a:	75 f4                	jne    106360 <strxfrm+0x20>
  10636c:	83 c2 02             	add    edx,0x2
    (void)memcpy((void *)dest, (void *)src, min(n, len + 1));
  10636f:	39 d6                	cmp    esi,edx
  106371:	0f 46 d6             	cmovbe edx,esi
    for (size_t i = 0; i < n; i++)
  106374:	85 d2                	test   edx,edx
  106376:	89 14 24             	mov    DWORD PTR [esp],edx
  106379:	74 7f                	je     1063fa <strxfrm+0xba>
  10637b:	8d 71 04             	lea    esi,[ecx+0x4]
  10637e:	8d 7d 04             	lea    edi,[ebp+0x4]
  106381:	39 f5                	cmp    ebp,esi
  106383:	0f 93 c2             	setae  dl
  106386:	39 f9                	cmp    ecx,edi
  106388:	89 d6                	mov    esi,edx
  10638a:	0f 93 c2             	setae  dl
  10638d:	89 d7                	mov    edi,edx
  10638f:	09 fe                	or     esi,edi
  106391:	89 cf                	mov    edi,ecx
  106393:	09 ef                	or     edi,ebp
  106395:	83 e7 03             	and    edi,0x3
  106398:	0f 94 c2             	sete   dl
  10639b:	89 d7                	mov    edi,edx
  10639d:	89 f2                	mov    edx,esi
  10639f:	89 fb                	mov    ebx,edi
  1063a1:	84 da                	test   dl,bl
  1063a3:	74 63                	je     106408 <strxfrm+0xc8>
  1063a5:	8b 1c 24             	mov    ebx,DWORD PTR [esp]
  1063a8:	83 fb 09             	cmp    ebx,0x9
  1063ab:	76 5b                	jbe    106408 <strxfrm+0xc8>
  1063ad:	89 df                	mov    edi,ebx
  1063af:	31 f6                	xor    esi,esi
  1063b1:	c1 ef 02             	shr    edi,0x2
  1063b4:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
        d[i] = s[i];
  1063b8:	8b 1c b1             	mov    ebx,DWORD PTR [ecx+esi*4]
  1063bb:	89 5c b5 00          	mov    DWORD PTR [ebp+esi*4+0x0],ebx
  1063bf:	83 c6 01             	add    esi,0x1
  1063c2:	39 f7                	cmp    edi,esi
  1063c4:	77 f2                	ja     1063b8 <strxfrm+0x78>
  1063c6:	8b 14 24             	mov    edx,DWORD PTR [esp]
  1063c9:	89 d6                	mov    esi,edx
  1063cb:	83 e6 fc             	and    esi,0xfffffffc
  1063ce:	39 f2                	cmp    edx,esi
  1063d0:	74 28                	je     1063fa <strxfrm+0xba>
  1063d2:	0f b6 1c 31          	movzx  ebx,BYTE PTR [ecx+esi*1]
    for (size_t i = 0; i < n; i++)
  1063d6:	8d 7e 01             	lea    edi,[esi+0x1]
  1063d9:	39 fa                	cmp    edx,edi
        d[i] = s[i];
  1063db:	88 5c 35 00          	mov    BYTE PTR [ebp+esi*1+0x0],bl
    for (size_t i = 0; i < n; i++)
  1063df:	76 19                	jbe    1063fa <strxfrm+0xba>
        d[i] = s[i];
  1063e1:	0f b6 5c 31 01       	movzx  ebx,BYTE PTR [ecx+esi*1+0x1]
    for (size_t i = 0; i < n; i++)
  1063e6:	8d 7e 02             	lea    edi,[esi+0x2]
  1063e9:	39 fa                	cmp    edx,edi
        d[i] = s[i];
  1063eb:	88 5c 35 01          	mov    BYTE PTR [ebp+esi*1+0x1],bl
    for (size_t i = 0; i < n; i++)
  1063ef:	76 09                	jbe    1063fa <strxfrm+0xba>
        d[i] = s[i];
  1063f1:	0f b6 54 31 02       	movzx  edx,BYTE PTR [ecx+esi*1+0x2]
  1063f6:	88 54 35 02          	mov    BYTE PTR [ebp+esi*1+0x2],dl

    return len;
}
  1063fa:	83 c4 04             	add    esp,0x4
  1063fd:	5b                   	pop    ebx
  1063fe:	5e                   	pop    esi
  1063ff:	5f                   	pop    edi
  106400:	5d                   	pop    ebp
  106401:	c3                   	ret    
  106402:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
    for (size_t i = 0; i < n; i++)
  106408:	31 f6                	xor    esi,esi
  10640a:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
        d[i] = s[i];
  106410:	0f b6 14 31          	movzx  edx,BYTE PTR [ecx+esi*1]
  106414:	88 54 35 00          	mov    BYTE PTR [ebp+esi*1+0x0],dl
    for (size_t i = 0; i < n; i++)
  106418:	83 c6 01             	add    esi,0x1
  10641b:	39 34 24             	cmp    DWORD PTR [esp],esi
  10641e:	75 f0                	jne    106410 <strxfrm+0xd0>
}
  106420:	83 c4 04             	add    esp,0x4
  106423:	5b                   	pop    ebx
  106424:	5e                   	pop    esi
  106425:	5f                   	pop    edi
  106426:	5d                   	pop    ebp
  106427:	c3                   	ret    
  106428:	90                   	nop
  106429:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
    while (str[lenght])
  106430:	ba 01 00 00 00       	mov    edx,0x1
    size_t lenght = 0;
  106435:	31 c0                	xor    eax,eax
  106437:	e9 33 ff ff ff       	jmp    10636f <strxfrm+0x2f>
  10643c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]

00106440 <strapd>:

void strapd(char * str, char c)
{
  106440:	57                   	push   edi
  106441:	56                   	push   esi
  106442:	53                   	push   ebx
  106443:	8b 74 24 10          	mov    esi,DWORD PTR [esp+0x10]
  106447:	8b 7c 24 14          	mov    edi,DWORD PTR [esp+0x14]
    while (str[lenght])
  10644b:	80 3e 00             	cmp    BYTE PTR [esi],0x0
  10644e:	74 30                	je     106480 <strapd+0x40>
  106450:	8d 46 01             	lea    eax,[esi+0x1]
    size_t lenght = 0;
  106453:	31 d2                	xor    edx,edx
  106455:	eb 0b                	jmp    106462 <strapd+0x22>
  106457:	89 f6                	mov    esi,esi
  106459:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
        lenght++;
  106460:	89 da                	mov    edx,ebx
    while (str[lenght])
  106462:	89 c1                	mov    ecx,eax
  106464:	8d 40 01             	lea    eax,[eax+0x1]
  106467:	80 78 ff 00          	cmp    BYTE PTR [eax-0x1],0x0
        lenght++;
  10646b:	8d 5a 01             	lea    ebx,[edx+0x1]
    while (str[lenght])
  10646e:	75 f0                	jne    106460 <strapd+0x20>
    unsigned int len = strlen(str);
    str[len] = c;
  106470:	89 f8                	mov    eax,edi
  106472:	83 c2 02             	add    edx,0x2
  106475:	88 01                	mov    BYTE PTR [ecx],al
    str[len + 1] = '\0';
  106477:	c6 04 16 00          	mov    BYTE PTR [esi+edx*1],0x0
}
  10647b:	5b                   	pop    ebx
  10647c:	5e                   	pop    esi
  10647d:	5f                   	pop    edi
  10647e:	c3                   	ret    
  10647f:	90                   	nop
    while (str[lenght])
  106480:	89 f1                	mov    ecx,esi
    str[len] = c;
  106482:	89 f8                	mov    eax,edi
    while (str[lenght])
  106484:	ba 01 00 00 00       	mov    edx,0x1
    str[len] = c;
  106489:	88 01                	mov    BYTE PTR [ecx],al
    str[len + 1] = '\0';
  10648b:	c6 04 16 00          	mov    BYTE PTR [esi+edx*1],0x0
}
  10648f:	5b                   	pop    ebx
  106490:	5e                   	pop    esi
  106491:	5f                   	pop    edi
  106492:	c3                   	ret    
  106493:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
  106499:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

001064a0 <strrvs>:

void strrvs(char * str)
{
  1064a0:	56                   	push   esi
  1064a1:	53                   	push   ebx
  1064a2:	8b 54 24 0c          	mov    edx,DWORD PTR [esp+0xc]
    while (str[lenght])
  1064a6:	80 3a 00             	cmp    BYTE PTR [edx],0x0
  1064a9:	74 37                	je     1064e2 <strrvs+0x42>
    size_t lenght = 0;
  1064ab:	31 c0                	xor    eax,eax
  1064ad:	eb 03                	jmp    1064b2 <strrvs+0x12>
  1064af:	90                   	nop
        lenght++;
  1064b0:	89 c8                	mov    eax,ecx
    while (str[lenght])
  1064b2:	80 7c 02 01 00       	cmp    BYTE PTR [edx+eax*1+0x1],0x0
        lenght++;
  1064b7:	8d 48 01             	lea    ecx,[eax+0x1]
    while (str[lenght])
  1064ba:	75 f4                	jne    1064b0 <strrvs+0x10>
    int c, i, j;
    for (i = 0, j = strlen(str) - 1; i < j; i++, j--)
  1064bc:	85 c0                	test   eax,eax
  1064be:	7e 22                	jle    1064e2 <strrvs+0x42>
  1064c0:	31 c9                	xor    ecx,ecx
  1064c2:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
    {
        c = str[i];
  1064c8:	0f b6 34 0a          	movzx  esi,BYTE PTR [edx+ecx*1]
        str[i] = str[j];
  1064cc:	0f b6 1c 02          	movzx  ebx,BYTE PTR [edx+eax*1]
  1064d0:	88 1c 0a             	mov    BYTE PTR [edx+ecx*1],bl
        str[j] = c;
  1064d3:	89 f3                	mov    ebx,esi
    for (i = 0, j = strlen(str) - 1; i < j; i++, j--)
  1064d5:	83 c1 01             	add    ecx,0x1
        str[j] = c;
  1064d8:	88 1c 02             	mov    BYTE PTR [edx+eax*1],bl
    for (i = 0, j = strlen(str) - 1; i < j; i++, j--)
  1064db:	83 e8 01             	sub    eax,0x1
  1064de:	39 c1                	cmp    ecx,eax
  1064e0:	7c e6                	jl     1064c8 <strrvs+0x28>
    }
}
  1064e2:	5b                   	pop    ebx
  1064e3:	5e                   	pop    esi
  1064e4:	c3                   	ret    
  1064e5:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
  1064e9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

001064f0 <strleadtrim>:

void strleadtrim(char * str, char c)
{
  1064f0:	55                   	push   ebp
  1064f1:	57                   	push   edi
  1064f2:	56                   	push   esi
  1064f3:	53                   	push   ebx
  1064f4:	8b 6c 24 14          	mov    ebp,DWORD PTR [esp+0x14]
  1064f8:	8b 54 24 18          	mov    edx,DWORD PTR [esp+0x18]
    char *start = str;
    while (*start == c)
  1064fc:	3a 55 00             	cmp    dl,BYTE PTR [ebp+0x0]
  1064ff:	0f 85 d3 00 00 00    	jne    1065d8 <strleadtrim+0xe8>
  106505:	89 d0                	mov    eax,edx
  106507:	89 ea                	mov    edx,ebp
  106509:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
        start++;
  106510:	83 c2 01             	add    edx,0x1
    while (*start == c)
  106513:	38 02                	cmp    BYTE PTR [edx],al
  106515:	74 f9                	je     106510 <strleadtrim+0x20>
    size_t lenght = 0;
  106517:	31 c0                	xor    eax,eax
  106519:	eb 07                	jmp    106522 <strleadtrim+0x32>
  10651b:	90                   	nop
  10651c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
        lenght++;
  106520:	89 d8                	mov    eax,ebx
    while (str[lenght])
  106522:	80 7c 05 01 00       	cmp    BYTE PTR [ebp+eax*1+0x1],0x0
  106527:	8d 58 01             	lea    ebx,[eax+0x1]
  10652a:	75 f4                	jne    106520 <strleadtrim+0x30>
    if (udest < usrc)
  10652c:	39 d5                	cmp    ebp,edx
  10652e:	0f 83 84 00 00 00    	jae    1065b8 <strleadtrim+0xc8>
        for (i = 0; i < n; i++)
  106534:	85 c0                	test   eax,eax
  106536:	74 77                	je     1065af <strleadtrim+0xbf>
  106538:	8d 5d 04             	lea    ebx,[ebp+0x4]
  10653b:	8d 72 04             	lea    esi,[edx+0x4]
  10653e:	39 da                	cmp    edx,ebx
  106540:	0f 93 c1             	setae  cl
  106543:	39 f5                	cmp    ebp,esi
  106545:	0f 93 c3             	setae  bl
  106548:	89 cf                	mov    edi,ecx
  10654a:	89 de                	mov    esi,ebx
  10654c:	09 f7                	or     edi,esi
  10654e:	89 ee                	mov    esi,ebp
  106550:	09 d6                	or     esi,edx
  106552:	89 f9                	mov    ecx,edi
  106554:	83 e6 03             	and    esi,0x3
  106557:	0f 94 c3             	sete   bl
  10655a:	84 d9                	test   cl,bl
  10655c:	0f 84 7e 00 00 00    	je     1065e0 <strleadtrim+0xf0>
  106562:	83 f8 09             	cmp    eax,0x9
  106565:	76 79                	jbe    1065e0 <strleadtrim+0xf0>
  106567:	89 c6                	mov    esi,eax
  106569:	31 db                	xor    ebx,ebx
  10656b:	c1 ee 02             	shr    esi,0x2
  10656e:	66 90                	xchg   ax,ax
            udest[i] = usrc[i];
  106570:	8b 3c 9a             	mov    edi,DWORD PTR [edx+ebx*4]
  106573:	89 7c 9d 00          	mov    DWORD PTR [ebp+ebx*4+0x0],edi
  106577:	83 c3 01             	add    ebx,0x1
  10657a:	39 de                	cmp    esi,ebx
  10657c:	77 f2                	ja     106570 <strleadtrim+0x80>
  10657e:	89 c6                	mov    esi,eax
  106580:	83 e6 fc             	and    esi,0xfffffffc
  106583:	39 f0                	cmp    eax,esi
  106585:	74 28                	je     1065af <strleadtrim+0xbf>
  106587:	0f b6 1c 32          	movzx  ebx,BYTE PTR [edx+esi*1]
  10658b:	88 5c 35 00          	mov    BYTE PTR [ebp+esi*1+0x0],bl
        for (i = 0; i < n; i++)
  10658f:	8d 5e 01             	lea    ebx,[esi+0x1]
  106592:	39 d8                	cmp    eax,ebx
  106594:	76 19                	jbe    1065af <strleadtrim+0xbf>
            udest[i] = usrc[i];
  106596:	0f b6 5c 32 01       	movzx  ebx,BYTE PTR [edx+esi*1+0x1]
  10659b:	88 5c 35 01          	mov    BYTE PTR [ebp+esi*1+0x1],bl
        for (i = 0; i < n; i++)
  10659f:	8d 5e 02             	lea    ebx,[esi+0x2]
  1065a2:	39 d8                	cmp    eax,ebx
  1065a4:	76 09                	jbe    1065af <strleadtrim+0xbf>
            udest[i] = usrc[i];
  1065a6:	0f b6 44 32 02       	movzx  eax,BYTE PTR [edx+esi*1+0x2]
  1065ab:	88 44 35 02          	mov    BYTE PTR [ebp+esi*1+0x2],al
    memmove(str, start, strlen(str + 1));
}
  1065af:	5b                   	pop    ebx
  1065b0:	5e                   	pop    esi
  1065b1:	5f                   	pop    edi
  1065b2:	5d                   	pop    ebp
  1065b3:	c3                   	ret    
  1065b4:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
    else if (udest > usrc)
  1065b8:	76 f5                	jbe    1065af <strleadtrim+0xbf>
        for (i = n; i > 0; i--)
  1065ba:	85 c0                	test   eax,eax
  1065bc:	74 f1                	je     1065af <strleadtrim+0xbf>
  1065be:	66 90                	xchg   ax,ax
            udest[i - 1] = usrc[i - 1];
  1065c0:	83 e8 01             	sub    eax,0x1
  1065c3:	0f b6 1c 02          	movzx  ebx,BYTE PTR [edx+eax*1]
        for (i = n; i > 0; i--)
  1065c7:	85 c0                	test   eax,eax
            udest[i - 1] = usrc[i - 1];
  1065c9:	88 5c 05 00          	mov    BYTE PTR [ebp+eax*1+0x0],bl
        for (i = n; i > 0; i--)
  1065cd:	75 f1                	jne    1065c0 <strleadtrim+0xd0>
}
  1065cf:	5b                   	pop    ebx
  1065d0:	5e                   	pop    esi
  1065d1:	5f                   	pop    edi
  1065d2:	5d                   	pop    ebp
  1065d3:	c3                   	ret    
  1065d4:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
    while (*start == c)
  1065d8:	89 ea                	mov    edx,ebp
  1065da:	e9 38 ff ff ff       	jmp    106517 <strleadtrim+0x27>
  1065df:	90                   	nop
        for (i = 0; i < n; i++)
  1065e0:	31 f6                	xor    esi,esi
  1065e2:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
            udest[i] = usrc[i];
  1065e8:	0f b6 1c 32          	movzx  ebx,BYTE PTR [edx+esi*1]
  1065ec:	88 5c 35 00          	mov    BYTE PTR [ebp+esi*1+0x0],bl
        for (i = 0; i < n; i++)
  1065f0:	83 c6 01             	add    esi,0x1
  1065f3:	39 f0                	cmp    eax,esi
  1065f5:	75 f1                	jne    1065e8 <strleadtrim+0xf8>
  1065f7:	eb b6                	jmp    1065af <strleadtrim+0xbf>
  1065f9:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]

00106600 <strtrailtrim>:

void strtrailtrim(char * str, char c)
{
  106600:	56                   	push   esi
  106601:	53                   	push   ebx
  106602:	8b 54 24 0c          	mov    edx,DWORD PTR [esp+0xc]
  106606:	8b 74 24 10          	mov    esi,DWORD PTR [esp+0x10]
    while (str[lenght])
  10660a:	80 3a 00             	cmp    BYTE PTR [edx],0x0
{
  10660d:	89 f3                	mov    ebx,esi
    while (str[lenght])
  10660f:	74 4f                	je     106660 <strtrailtrim+0x60>
    size_t lenght = 0;
  106611:	31 c0                	xor    eax,eax
  106613:	eb 05                	jmp    10661a <strtrailtrim+0x1a>
  106615:	8d 76 00             	lea    esi,[esi+0x0]
        lenght++;
  106618:	89 c8                	mov    eax,ecx
    while (str[lenght])
  10661a:	80 7c 02 01 00       	cmp    BYTE PTR [edx+eax*1+0x1],0x0
        lenght++;
  10661f:	8d 48 01             	lea    ecx,[eax+0x1]
    while (str[lenght])
  106622:	75 f4                	jne    106618 <strtrailtrim+0x18>
    char *end = str + strlen(str) - 1;
  106624:	01 d0                	add    eax,edx

    while (end > str && *end == c)
  106626:	39 c2                	cmp    edx,eax
  106628:	73 26                	jae    106650 <strtrailtrim+0x50>
  10662a:	89 f1                	mov    ecx,esi
  10662c:	3a 08                	cmp    cl,BYTE PTR [eax]
  10662e:	74 0c                	je     10663c <strtrailtrim+0x3c>
  106630:	eb 1e                	jmp    106650 <strtrailtrim+0x50>
  106632:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
  106638:	38 18                	cmp    BYTE PTR [eax],bl
  10663a:	75 14                	jne    106650 <strtrailtrim+0x50>
        end--;
  10663c:	83 e8 01             	sub    eax,0x1
    while (end > str && *end == c)
  10663f:	39 c2                	cmp    edx,eax
  106641:	75 f5                	jne    106638 <strtrailtrim+0x38>

    end[1] = '\0';
  106643:	c6 42 01 00          	mov    BYTE PTR [edx+0x1],0x0
  106647:	5b                   	pop    ebx
  106648:	5e                   	pop    esi
  106649:	c3                   	ret    
  10664a:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
    while (end > str && *end == c)
  106650:	89 c2                	mov    edx,eax
    end[1] = '\0';
  106652:	c6 42 01 00          	mov    BYTE PTR [edx+0x1],0x0
  106656:	5b                   	pop    ebx
  106657:	5e                   	pop    esi
  106658:	c3                   	ret    
  106659:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
    while (str[lenght])
  106660:	b8 ff ff ff ff       	mov    eax,0xffffffff
  106665:	eb bd                	jmp    106624 <strtrailtrim+0x24>
  106667:	66 90                	xchg   ax,ax
  106669:	66 90                	xchg   ax,ax
  10666b:	66 90                	xchg   ax,ax
  10666d:	66 90                	xchg   ax,ax
  10666f:	90                   	nop

00106670 <printf>:
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

int printf(const char *format, ...)
{
  106670:	55                   	push   ebp
  106671:	89 e5                	mov    ebp,esp
  106673:	57                   	push   edi
  106674:	56                   	push   esi
  106675:	53                   	push   ebx
  106676:	83 ec 48             	sub    esp,0x48
    va_list va;
    va_start(va, format);

    int result = vprintf(format, va);
  106679:	8b 5d 08             	mov    ebx,DWORD PTR [ebp+0x8]
    return result;
}

int vprintf(const char *format, va_list va)
{
    char buffer[strlen(format) + 128];
  10667c:	53                   	push   ebx
  10667d:	e8 3e fa ff ff       	call   1060c0 <strlen>
  106682:	05 8f 00 00 00       	add    eax,0x8f
  106687:	83 c4 10             	add    esp,0x10
  10668a:	83 e0 f0             	and    eax,0xfffffff0
  10668d:	29 c4                	sub    esp,eax
{
    str[0] = '\0';

    bool wait_for_format = false;

    for (unsigned int i = 0; format[i]; i++)
  10668f:	0f b6 03             	movzx  eax,BYTE PTR [ebx]
    char buffer[strlen(format) + 128];
  106692:	89 e6                	mov    esi,esp
    str[0] = '\0';
  106694:	c6 04 24 00          	mov    BYTE PTR [esp],0x0
    for (unsigned int i = 0; format[i]; i++)
  106698:	84 c0                	test   al,al
  10669a:	0f 84 94 00 00 00    	je     106734 <printf+0xc4>
  1066a0:	83 c3 01             	add    ebx,0x1
  1066a3:	8d 7d 0c             	lea    edi,[ebp+0xc]
    bool wait_for_format = false;
  1066a6:	31 d2                	xor    edx,edx
  1066a8:	eb 1a                	jmp    1066c4 <printf+0x54>
  1066aa:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
    {
        char c = format[i];

        if (c == '%' && !wait_for_format)
  1066b0:	84 d2                	test   dl,dl
  1066b2:	75 14                	jne    1066c8 <printf+0x58>
        {
            wait_for_format = true;
  1066b4:	ba 01 00 00 00       	mov    edx,0x1
  1066b9:	83 c3 01             	add    ebx,0x1
    for (unsigned int i = 0; format[i]; i++)
  1066bc:	0f b6 43 ff          	movzx  eax,BYTE PTR [ebx-0x1]
  1066c0:	84 c0                	test   al,al
  1066c2:	74 70                	je     106734 <printf+0xc4>
        if (c == '%' && !wait_for_format)
  1066c4:	3c 25                	cmp    al,0x25
  1066c6:	74 e8                	je     1066b0 <printf+0x40>
        }
        else if (wait_for_format)
  1066c8:	84 d2                	test   dl,dl
  1066ca:	0f be c8             	movsx  ecx,al
  1066cd:	0f 84 85 00 00 00    	je     106758 <printf+0xe8>
        {
            char temp[32];
            switch (c)
  1066d3:	3c 64                	cmp    al,0x64
  1066d5:	0f 84 75 01 00 00    	je     106850 <printf+0x1e0>
  1066db:	0f 8e 97 00 00 00    	jle    106778 <printf+0x108>
  1066e1:	3c 73                	cmp    al,0x73
  1066e3:	0f 84 47 01 00 00    	je     106830 <printf+0x1c0>
  1066e9:	0f 8e e1 00 00 00    	jle    1067d0 <printf+0x160>
  1066ef:	3c 75                	cmp    al,0x75
  1066f1:	0f 84 c9 00 00 00    	je     1067c0 <printf+0x150>
  1066f7:	3c 78                	cmp    al,0x78
  1066f9:	0f 85 d5 00 00 00    	jne    1067d4 <printf+0x164>
                    itos(va_arg(va, unsigned int), temp, 10);
                    strcat(str, temp);
                    break;

                case 'x':
                    itos(va_arg(va, unsigned int), temp, 16);
  1066ff:	8d 47 04             	lea    eax,[edi+0x4]
  106702:	83 ec 04             	sub    esp,0x4
  106705:	6a 10                	push   0x10
  106707:	89 45 c4             	mov    DWORD PTR [ebp-0x3c],eax
                    itos(va_arg(va, unsigned int), temp, 10);
  10670a:	8d 4d c8             	lea    ecx,[ebp-0x38]
  10670d:	51                   	push   ecx
  10670e:	ff 37                	push   DWORD PTR [edi]
  106710:	e8 1b 08 00 00       	call   106f30 <itos>
                    strcat(str, temp);
  106715:	59                   	pop    ecx
  106716:	8d 4d c8             	lea    ecx,[ebp-0x38]
  106719:	5f                   	pop    edi
  10671a:	51                   	push   ecx
  10671b:	56                   	push   esi
  10671c:	e8 bf f1 ff ff       	call   1058e0 <strcat>
                    itos(va_arg(va, unsigned int), temp, 10);
  106721:	8b 7d c4             	mov    edi,DWORD PTR [ebp-0x3c]
                    strcat(str, temp);
  106724:	83 c4 10             	add    esp,0x10
                    strapd(str, '%');
                    strapd(str, c);
                    break;
            }

            wait_for_format = false;
  106727:	31 d2                	xor    edx,edx
  106729:	83 c3 01             	add    ebx,0x1
    for (unsigned int i = 0; format[i]; i++)
  10672c:	0f b6 43 ff          	movzx  eax,BYTE PTR [ebx-0x1]
  106730:	84 c0                	test   al,al
  106732:	75 90                	jne    1066c4 <printf+0x54>
        {
            strapd(str, c);
        }
    }

    return strlen(str);
  106734:	83 ec 0c             	sub    esp,0xc
  106737:	56                   	push   esi
  106738:	e8 83 f9 ff ff       	call   1060c0 <strlen>
    puts(buffer);
  10673d:	89 34 24             	mov    DWORD PTR [esp],esi
    return strlen(str);
  106740:	89 c3                	mov    ebx,eax
    puts(buffer);
  106742:	e8 59 11 00 00       	call   1078a0 <puts>
}
  106747:	8d 65 f4             	lea    esp,[ebp-0xc]
  10674a:	89 d8                	mov    eax,ebx
  10674c:	5b                   	pop    ebx
  10674d:	5e                   	pop    esi
  10674e:	5f                   	pop    edi
  10674f:	5d                   	pop    ebp
  106750:	c3                   	ret    
  106751:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
            strapd(str, c);
  106758:	83 ec 08             	sub    esp,0x8
  10675b:	88 55 c4             	mov    BYTE PTR [ebp-0x3c],dl
  10675e:	51                   	push   ecx
  10675f:	56                   	push   esi
  106760:	e8 db fc ff ff       	call   106440 <strapd>
  106765:	83 c4 10             	add    esp,0x10
  106768:	0f b6 55 c4          	movzx  edx,BYTE PTR [ebp-0x3c]
  10676c:	e9 48 ff ff ff       	jmp    1066b9 <printf+0x49>
  106771:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
            switch (c)
  106778:	3c 62                	cmp    al,0x62
  10677a:	0f 84 80 00 00 00    	je     106800 <printf+0x190>
  106780:	7f 1e                	jg     1067a0 <printf+0x130>
  106782:	3c 25                	cmp    al,0x25
  106784:	75 4e                	jne    1067d4 <printf+0x164>
                    strapd(str, '%');
  106786:	83 ec 08             	sub    esp,0x8
  106789:	6a 25                	push   0x25
  10678b:	56                   	push   esi
  10678c:	e8 af fc ff ff       	call   106440 <strapd>
  106791:	83 c4 10             	add    esp,0x10
            wait_for_format = false;
  106794:	31 d2                	xor    edx,edx
  106796:	eb 91                	jmp    106729 <printf+0xb9>
  106798:	90                   	nop
  106799:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
                    strapd(str, va_arg(va, char));
  1067a0:	8d 47 04             	lea    eax,[edi+0x4]
  1067a3:	83 ec 08             	sub    esp,0x8
  1067a6:	89 45 c4             	mov    DWORD PTR [ebp-0x3c],eax
  1067a9:	0f be 07             	movsx  eax,BYTE PTR [edi]
  1067ac:	50                   	push   eax
  1067ad:	56                   	push   esi
  1067ae:	e8 8d fc ff ff       	call   106440 <strapd>
  1067b3:	83 c4 10             	add    esp,0x10
  1067b6:	8b 7d c4             	mov    edi,DWORD PTR [ebp-0x3c]
            wait_for_format = false;
  1067b9:	31 d2                	xor    edx,edx
  1067bb:	e9 69 ff ff ff       	jmp    106729 <printf+0xb9>
                    itos(va_arg(va, unsigned int), temp, 10);
  1067c0:	8d 47 04             	lea    eax,[edi+0x4]
  1067c3:	83 ec 04             	sub    esp,0x4
  1067c6:	6a 0a                	push   0xa
  1067c8:	89 45 c4             	mov    DWORD PTR [ebp-0x3c],eax
  1067cb:	e9 3a ff ff ff       	jmp    10670a <printf+0x9a>
            switch (c)
  1067d0:	3c 69                	cmp    al,0x69
  1067d2:	74 7c                	je     106850 <printf+0x1e0>
                    strapd(str, '%');
  1067d4:	83 ec 08             	sub    esp,0x8
  1067d7:	89 4d c4             	mov    DWORD PTR [ebp-0x3c],ecx
  1067da:	6a 25                	push   0x25
  1067dc:	56                   	push   esi
  1067dd:	e8 5e fc ff ff       	call   106440 <strapd>
                    strapd(str, c);
  1067e2:	8b 4d c4             	mov    ecx,DWORD PTR [ebp-0x3c]
  1067e5:	58                   	pop    eax
  1067e6:	5a                   	pop    edx
  1067e7:	51                   	push   ecx
  1067e8:	56                   	push   esi
  1067e9:	e8 52 fc ff ff       	call   106440 <strapd>
  1067ee:	83 c4 10             	add    esp,0x10
            wait_for_format = false;
  1067f1:	31 d2                	xor    edx,edx
  1067f3:	e9 31 ff ff ff       	jmp    106729 <printf+0xb9>
  1067f8:	90                   	nop
  1067f9:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
                    itos(va_arg(va, unsigned int), temp, 2);
  106800:	8d 4d c8             	lea    ecx,[ebp-0x38]
  106803:	83 ec 04             	sub    esp,0x4
  106806:	8d 47 04             	lea    eax,[edi+0x4]
  106809:	6a 02                	push   0x2
  10680b:	51                   	push   ecx
  10680c:	ff 37                	push   DWORD PTR [edi]
  10680e:	89 45 c4             	mov    DWORD PTR [ebp-0x3c],eax
  106811:	e8 1a 07 00 00       	call   106f30 <itos>
                    strcat(str, "0b");
  106816:	59                   	pop    ecx
  106817:	5f                   	pop    edi
  106818:	68 91 24 11 00       	push   0x112491
  10681d:	56                   	push   esi
  10681e:	e8 bd f0 ff ff       	call   1058e0 <strcat>
  106823:	e9 ed fe ff ff       	jmp    106715 <printf+0xa5>
  106828:	90                   	nop
  106829:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
                    strcat(str, va_arg(va, const char *));
  106830:	83 ec 08             	sub    esp,0x8
  106833:	8d 47 04             	lea    eax,[edi+0x4]
  106836:	ff 37                	push   DWORD PTR [edi]
  106838:	56                   	push   esi
  106839:	89 45 c4             	mov    DWORD PTR [ebp-0x3c],eax
  10683c:	e8 9f f0 ff ff       	call   1058e0 <strcat>
  106841:	83 c4 10             	add    esp,0x10
  106844:	8b 7d c4             	mov    edi,DWORD PTR [ebp-0x3c]
            wait_for_format = false;
  106847:	31 d2                	xor    edx,edx
  106849:	e9 db fe ff ff       	jmp    106729 <printf+0xb9>
  10684e:	66 90                	xchg   ax,ax
                        int value = va_arg(va, unsigned int);
  106850:	8d 47 04             	lea    eax,[edi+0x4]
  106853:	8b 3f                	mov    edi,DWORD PTR [edi]
  106855:	89 45 c4             	mov    DWORD PTR [ebp-0x3c],eax
                        if (value < 0)
  106858:	85 ff                	test   edi,edi
  10685a:	78 28                	js     106884 <printf+0x214>
                        itos(uvalue, temp, 10);
  10685c:	8d 45 c8             	lea    eax,[ebp-0x38]
  10685f:	83 ec 04             	sub    esp,0x4
  106862:	6a 0a                	push   0xa
  106864:	50                   	push   eax
  106865:	57                   	push   edi
  106866:	e8 c5 06 00 00       	call   106f30 <itos>
                        strcat(str, temp);
  10686b:	58                   	pop    eax
  10686c:	8d 45 c8             	lea    eax,[ebp-0x38]
  10686f:	5a                   	pop    edx
  106870:	50                   	push   eax
  106871:	56                   	push   esi
  106872:	e8 69 f0 ff ff       	call   1058e0 <strcat>
  106877:	83 c4 10             	add    esp,0x10
                        int value = va_arg(va, unsigned int);
  10687a:	8b 7d c4             	mov    edi,DWORD PTR [ebp-0x3c]
            wait_for_format = false;
  10687d:	31 d2                	xor    edx,edx
  10687f:	e9 a5 fe ff ff       	jmp    106729 <printf+0xb9>
                            strcat(str, "-");
  106884:	83 ec 08             	sub    esp,0x8
                            uvalue = 0 - value;
  106887:	f7 df                	neg    edi
                            strcat(str, "-");
  106889:	68 f1 21 11 00       	push   0x1121f1
  10688e:	56                   	push   esi
  10688f:	e8 4c f0 ff ff       	call   1058e0 <strcat>
  106894:	83 c4 10             	add    esp,0x10
  106897:	eb c3                	jmp    10685c <printf+0x1ec>
  106899:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]

001068a0 <vprintf>:
{
  1068a0:	55                   	push   ebp
  1068a1:	89 e5                	mov    ebp,esp
  1068a3:	57                   	push   edi
  1068a4:	56                   	push   esi
  1068a5:	53                   	push   ebx
  1068a6:	83 ec 48             	sub    esp,0x48
  1068a9:	8b 7d 08             	mov    edi,DWORD PTR [ebp+0x8]
  1068ac:	8b 75 0c             	mov    esi,DWORD PTR [ebp+0xc]
    char buffer[strlen(format) + 128];
  1068af:	57                   	push   edi
  1068b0:	e8 0b f8 ff ff       	call   1060c0 <strlen>
  1068b5:	05 8f 00 00 00       	add    eax,0x8f
  1068ba:	83 c4 10             	add    esp,0x10
  1068bd:	83 e0 f0             	and    eax,0xfffffff0
  1068c0:	29 c4                	sub    esp,eax
    for (unsigned int i = 0; format[i]; i++)
  1068c2:	0f b6 07             	movzx  eax,BYTE PTR [edi]
    char buffer[strlen(format) + 128];
  1068c5:	89 e3                	mov    ebx,esp
    str[0] = '\0';
  1068c7:	c6 04 24 00          	mov    BYTE PTR [esp],0x0
    for (unsigned int i = 0; format[i]; i++)
  1068cb:	84 c0                	test   al,al
  1068cd:	0f 84 91 00 00 00    	je     106964 <vprintf+0xc4>
  1068d3:	83 c7 01             	add    edi,0x1
    bool wait_for_format = false;
  1068d6:	31 d2                	xor    edx,edx
  1068d8:	eb 1a                	jmp    1068f4 <vprintf+0x54>
  1068da:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
        if (c == '%' && !wait_for_format)
  1068e0:	84 d2                	test   dl,dl
  1068e2:	75 14                	jne    1068f8 <vprintf+0x58>
            wait_for_format = true;
  1068e4:	ba 01 00 00 00       	mov    edx,0x1
  1068e9:	83 c7 01             	add    edi,0x1
    for (unsigned int i = 0; format[i]; i++)
  1068ec:	0f b6 47 ff          	movzx  eax,BYTE PTR [edi-0x1]
  1068f0:	84 c0                	test   al,al
  1068f2:	74 70                	je     106964 <vprintf+0xc4>
        if (c == '%' && !wait_for_format)
  1068f4:	3c 25                	cmp    al,0x25
  1068f6:	74 e8                	je     1068e0 <vprintf+0x40>
        else if (wait_for_format)
  1068f8:	84 d2                	test   dl,dl
  1068fa:	0f be c8             	movsx  ecx,al
  1068fd:	0f 84 85 00 00 00    	je     106988 <vprintf+0xe8>
            switch (c)
  106903:	3c 64                	cmp    al,0x64
  106905:	0f 84 75 01 00 00    	je     106a80 <vprintf+0x1e0>
  10690b:	0f 8e 97 00 00 00    	jle    1069a8 <vprintf+0x108>
  106911:	3c 73                	cmp    al,0x73
  106913:	0f 84 47 01 00 00    	je     106a60 <vprintf+0x1c0>
  106919:	0f 8e e1 00 00 00    	jle    106a00 <vprintf+0x160>
  10691f:	3c 75                	cmp    al,0x75
  106921:	0f 84 c9 00 00 00    	je     1069f0 <vprintf+0x150>
  106927:	3c 78                	cmp    al,0x78
  106929:	0f 85 d5 00 00 00    	jne    106a04 <vprintf+0x164>
                    itos(va_arg(va, unsigned int), temp, 16);
  10692f:	8d 46 04             	lea    eax,[esi+0x4]
  106932:	83 ec 04             	sub    esp,0x4
  106935:	6a 10                	push   0x10
  106937:	89 45 c4             	mov    DWORD PTR [ebp-0x3c],eax
                    itos(va_arg(va, unsigned int), temp, 10);
  10693a:	8d 4d c8             	lea    ecx,[ebp-0x38]
  10693d:	51                   	push   ecx
  10693e:	ff 36                	push   DWORD PTR [esi]
  106940:	e8 eb 05 00 00       	call   106f30 <itos>
                    strcat(str, temp);
  106945:	59                   	pop    ecx
  106946:	8d 4d c8             	lea    ecx,[ebp-0x38]
  106949:	5e                   	pop    esi
  10694a:	51                   	push   ecx
  10694b:	53                   	push   ebx
  10694c:	e8 8f ef ff ff       	call   1058e0 <strcat>
                    itos(va_arg(va, unsigned int), temp, 10);
  106951:	8b 75 c4             	mov    esi,DWORD PTR [ebp-0x3c]
                    strcat(str, temp);
  106954:	83 c4 10             	add    esp,0x10
            wait_for_format = false;
  106957:	31 d2                	xor    edx,edx
  106959:	83 c7 01             	add    edi,0x1
    for (unsigned int i = 0; format[i]; i++)
  10695c:	0f b6 47 ff          	movzx  eax,BYTE PTR [edi-0x1]
  106960:	84 c0                	test   al,al
  106962:	75 90                	jne    1068f4 <vprintf+0x54>
    return strlen(str);
  106964:	83 ec 0c             	sub    esp,0xc
  106967:	53                   	push   ebx
  106968:	e8 53 f7 ff ff       	call   1060c0 <strlen>
    puts(buffer);
  10696d:	89 1c 24             	mov    DWORD PTR [esp],ebx
    return strlen(str);
  106970:	89 c6                	mov    esi,eax
    puts(buffer);
  106972:	e8 29 0f 00 00       	call   1078a0 <puts>
}
  106977:	8d 65 f4             	lea    esp,[ebp-0xc]
  10697a:	89 f0                	mov    eax,esi
  10697c:	5b                   	pop    ebx
  10697d:	5e                   	pop    esi
  10697e:	5f                   	pop    edi
  10697f:	5d                   	pop    ebp
  106980:	c3                   	ret    
  106981:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
            strapd(str, c);
  106988:	83 ec 08             	sub    esp,0x8
  10698b:	88 55 c4             	mov    BYTE PTR [ebp-0x3c],dl
  10698e:	51                   	push   ecx
  10698f:	53                   	push   ebx
  106990:	e8 ab fa ff ff       	call   106440 <strapd>
  106995:	83 c4 10             	add    esp,0x10
  106998:	0f b6 55 c4          	movzx  edx,BYTE PTR [ebp-0x3c]
  10699c:	e9 48 ff ff ff       	jmp    1068e9 <vprintf+0x49>
  1069a1:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
            switch (c)
  1069a8:	3c 62                	cmp    al,0x62
  1069aa:	0f 84 80 00 00 00    	je     106a30 <vprintf+0x190>
  1069b0:	7f 1e                	jg     1069d0 <vprintf+0x130>
  1069b2:	3c 25                	cmp    al,0x25
  1069b4:	75 4e                	jne    106a04 <vprintf+0x164>
                    strapd(str, '%');
  1069b6:	83 ec 08             	sub    esp,0x8
  1069b9:	6a 25                	push   0x25
  1069bb:	53                   	push   ebx
  1069bc:	e8 7f fa ff ff       	call   106440 <strapd>
  1069c1:	83 c4 10             	add    esp,0x10
            wait_for_format = false;
  1069c4:	31 d2                	xor    edx,edx
  1069c6:	eb 91                	jmp    106959 <vprintf+0xb9>
  1069c8:	90                   	nop
  1069c9:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
                    strapd(str, va_arg(va, char));
  1069d0:	8d 46 04             	lea    eax,[esi+0x4]
  1069d3:	83 ec 08             	sub    esp,0x8
  1069d6:	89 45 c4             	mov    DWORD PTR [ebp-0x3c],eax
  1069d9:	0f be 06             	movsx  eax,BYTE PTR [esi]
  1069dc:	50                   	push   eax
  1069dd:	53                   	push   ebx
  1069de:	e8 5d fa ff ff       	call   106440 <strapd>
  1069e3:	83 c4 10             	add    esp,0x10
  1069e6:	8b 75 c4             	mov    esi,DWORD PTR [ebp-0x3c]
            wait_for_format = false;
  1069e9:	31 d2                	xor    edx,edx
  1069eb:	e9 69 ff ff ff       	jmp    106959 <vprintf+0xb9>
                    itos(va_arg(va, unsigned int), temp, 10);
  1069f0:	8d 46 04             	lea    eax,[esi+0x4]
  1069f3:	83 ec 04             	sub    esp,0x4
  1069f6:	6a 0a                	push   0xa
  1069f8:	89 45 c4             	mov    DWORD PTR [ebp-0x3c],eax
  1069fb:	e9 3a ff ff ff       	jmp    10693a <vprintf+0x9a>
            switch (c)
  106a00:	3c 69                	cmp    al,0x69
  106a02:	74 7c                	je     106a80 <vprintf+0x1e0>
                    strapd(str, '%');
  106a04:	83 ec 08             	sub    esp,0x8
  106a07:	89 4d c4             	mov    DWORD PTR [ebp-0x3c],ecx
  106a0a:	6a 25                	push   0x25
  106a0c:	53                   	push   ebx
  106a0d:	e8 2e fa ff ff       	call   106440 <strapd>
                    strapd(str, c);
  106a12:	8b 4d c4             	mov    ecx,DWORD PTR [ebp-0x3c]
  106a15:	58                   	pop    eax
  106a16:	5a                   	pop    edx
  106a17:	51                   	push   ecx
  106a18:	53                   	push   ebx
  106a19:	e8 22 fa ff ff       	call   106440 <strapd>
  106a1e:	83 c4 10             	add    esp,0x10
            wait_for_format = false;
  106a21:	31 d2                	xor    edx,edx
  106a23:	e9 31 ff ff ff       	jmp    106959 <vprintf+0xb9>
  106a28:	90                   	nop
  106a29:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
                    itos(va_arg(va, unsigned int), temp, 2);
  106a30:	8d 4d c8             	lea    ecx,[ebp-0x38]
  106a33:	83 ec 04             	sub    esp,0x4
  106a36:	8d 46 04             	lea    eax,[esi+0x4]
  106a39:	6a 02                	push   0x2
  106a3b:	51                   	push   ecx
  106a3c:	ff 36                	push   DWORD PTR [esi]
  106a3e:	89 45 c4             	mov    DWORD PTR [ebp-0x3c],eax
  106a41:	e8 ea 04 00 00       	call   106f30 <itos>
                    strcat(str, "0b");
  106a46:	59                   	pop    ecx
  106a47:	5e                   	pop    esi
  106a48:	68 91 24 11 00       	push   0x112491
  106a4d:	53                   	push   ebx
  106a4e:	e8 8d ee ff ff       	call   1058e0 <strcat>
  106a53:	e9 ed fe ff ff       	jmp    106945 <vprintf+0xa5>
  106a58:	90                   	nop
  106a59:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
                    strcat(str, va_arg(va, const char *));
  106a60:	83 ec 08             	sub    esp,0x8
  106a63:	8d 46 04             	lea    eax,[esi+0x4]
  106a66:	ff 36                	push   DWORD PTR [esi]
  106a68:	53                   	push   ebx
  106a69:	89 45 c4             	mov    DWORD PTR [ebp-0x3c],eax
  106a6c:	e8 6f ee ff ff       	call   1058e0 <strcat>
  106a71:	83 c4 10             	add    esp,0x10
  106a74:	8b 75 c4             	mov    esi,DWORD PTR [ebp-0x3c]
            wait_for_format = false;
  106a77:	31 d2                	xor    edx,edx
  106a79:	e9 db fe ff ff       	jmp    106959 <vprintf+0xb9>
  106a7e:	66 90                	xchg   ax,ax
                        int value = va_arg(va, unsigned int);
  106a80:	8d 46 04             	lea    eax,[esi+0x4]
  106a83:	8b 36                	mov    esi,DWORD PTR [esi]
  106a85:	89 45 c4             	mov    DWORD PTR [ebp-0x3c],eax
                        if (value < 0)
  106a88:	85 f6                	test   esi,esi
  106a8a:	78 28                	js     106ab4 <vprintf+0x214>
                        itos(uvalue, temp, 10);
  106a8c:	8d 45 c8             	lea    eax,[ebp-0x38]
  106a8f:	83 ec 04             	sub    esp,0x4
  106a92:	6a 0a                	push   0xa
  106a94:	50                   	push   eax
  106a95:	56                   	push   esi
  106a96:	e8 95 04 00 00       	call   106f30 <itos>
                        strcat(str, temp);
  106a9b:	58                   	pop    eax
  106a9c:	8d 45 c8             	lea    eax,[ebp-0x38]
  106a9f:	5a                   	pop    edx
  106aa0:	50                   	push   eax
  106aa1:	53                   	push   ebx
  106aa2:	e8 39 ee ff ff       	call   1058e0 <strcat>
  106aa7:	83 c4 10             	add    esp,0x10
                        int value = va_arg(va, unsigned int);
  106aaa:	8b 75 c4             	mov    esi,DWORD PTR [ebp-0x3c]
            wait_for_format = false;
  106aad:	31 d2                	xor    edx,edx
  106aaf:	e9 a5 fe ff ff       	jmp    106959 <vprintf+0xb9>
                            strcat(str, "-");
  106ab4:	83 ec 08             	sub    esp,0x8
                            uvalue = 0 - value;
  106ab7:	f7 de                	neg    esi
                            strcat(str, "-");
  106ab9:	68 f1 21 11 00       	push   0x1121f1
  106abe:	53                   	push   ebx
  106abf:	e8 1c ee ff ff       	call   1058e0 <strcat>
  106ac4:	83 c4 10             	add    esp,0x10
  106ac7:	eb c3                	jmp    106a8c <vprintf+0x1ec>
  106ac9:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]

00106ad0 <sprintf>:
{
  106ad0:	55                   	push   ebp
  106ad1:	57                   	push   edi
  106ad2:	56                   	push   esi
  106ad3:	53                   	push   ebx
  106ad4:	83 ec 3c             	sub    esp,0x3c
  106ad7:	8b 74 24 50          	mov    esi,DWORD PTR [esp+0x50]
    int result = vsprintf(str, format, va);
  106adb:	8b 5c 24 54          	mov    ebx,DWORD PTR [esp+0x54]
    str[0] = '\0';
  106adf:	c6 06 00             	mov    BYTE PTR [esi],0x0
    for (unsigned int i = 0; format[i]; i++)
  106ae2:	0f b6 03             	movzx  eax,BYTE PTR [ebx]
  106ae5:	84 c0                	test   al,al
  106ae7:	0f 84 91 00 00 00    	je     106b7e <sprintf+0xae>
  106aed:	83 c3 01             	add    ebx,0x1
  106af0:	8d 7c 24 58          	lea    edi,[esp+0x58]
    bool wait_for_format = false;
  106af4:	31 c9                	xor    ecx,ecx
  106af6:	eb 1c                	jmp    106b14 <sprintf+0x44>
  106af8:	90                   	nop
  106af9:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
        if (c == '%' && !wait_for_format)
  106b00:	84 c9                	test   cl,cl
  106b02:	75 14                	jne    106b18 <sprintf+0x48>
            wait_for_format = true;
  106b04:	b9 01 00 00 00       	mov    ecx,0x1
  106b09:	83 c3 01             	add    ebx,0x1
    for (unsigned int i = 0; format[i]; i++)
  106b0c:	0f b6 43 ff          	movzx  eax,BYTE PTR [ebx-0x1]
  106b10:	84 c0                	test   al,al
  106b12:	74 6a                	je     106b7e <sprintf+0xae>
        if (c == '%' && !wait_for_format)
  106b14:	3c 25                	cmp    al,0x25
  106b16:	74 e8                	je     106b00 <sprintf+0x30>
        else if (wait_for_format)
  106b18:	84 c9                	test   cl,cl
  106b1a:	0f be e8             	movsx  ebp,al
  106b1d:	74 71                	je     106b90 <sprintf+0xc0>
            switch (c)
  106b1f:	3c 64                	cmp    al,0x64
  106b21:	0f 84 59 01 00 00    	je     106c80 <sprintf+0x1b0>
  106b27:	0f 8e 83 00 00 00    	jle    106bb0 <sprintf+0xe0>
  106b2d:	3c 73                	cmp    al,0x73
  106b2f:	0f 84 2b 01 00 00    	je     106c60 <sprintf+0x190>
  106b35:	0f 8e cd 00 00 00    	jle    106c08 <sprintf+0x138>
  106b3b:	3c 75                	cmp    al,0x75
  106b3d:	0f 84 ad 00 00 00    	je     106bf0 <sprintf+0x120>
  106b43:	3c 78                	cmp    al,0x78
  106b45:	0f 85 c1 00 00 00    	jne    106c0c <sprintf+0x13c>
                    itos(va_arg(va, unsigned int), temp, 16);
  106b4b:	83 ec 04             	sub    esp,0x4
  106b4e:	8d 6f 04             	lea    ebp,[edi+0x4]
  106b51:	6a 10                	push   0x10
  106b53:	8d 44 24 18          	lea    eax,[esp+0x18]
  106b57:	50                   	push   eax
  106b58:	ff 37                	push   DWORD PTR [edi]
                        itos(uvalue, temp, 10);
  106b5a:	e8 d1 03 00 00       	call   106f30 <itos>
                        strcat(str, temp);
  106b5f:	59                   	pop    ecx
  106b60:	5f                   	pop    edi
                        int value = va_arg(va, unsigned int);
  106b61:	89 ef                	mov    edi,ebp
                        strcat(str, temp);
  106b63:	8d 44 24 18          	lea    eax,[esp+0x18]
  106b67:	50                   	push   eax
  106b68:	56                   	push   esi
  106b69:	e8 72 ed ff ff       	call   1058e0 <strcat>
  106b6e:	83 c4 10             	add    esp,0x10
            wait_for_format = false;
  106b71:	31 c9                	xor    ecx,ecx
  106b73:	83 c3 01             	add    ebx,0x1
    for (unsigned int i = 0; format[i]; i++)
  106b76:	0f b6 43 ff          	movzx  eax,BYTE PTR [ebx-0x1]
  106b7a:	84 c0                	test   al,al
  106b7c:	75 96                	jne    106b14 <sprintf+0x44>
    return strlen(str);
  106b7e:	83 ec 0c             	sub    esp,0xc
  106b81:	56                   	push   esi
  106b82:	e8 39 f5 ff ff       	call   1060c0 <strlen>
}
  106b87:	83 c4 4c             	add    esp,0x4c
  106b8a:	5b                   	pop    ebx
  106b8b:	5e                   	pop    esi
  106b8c:	5f                   	pop    edi
  106b8d:	5d                   	pop    ebp
  106b8e:	c3                   	ret    
  106b8f:	90                   	nop
  106b90:	88 4c 24 0f          	mov    BYTE PTR [esp+0xf],cl
            strapd(str, c);
  106b94:	83 ec 08             	sub    esp,0x8
  106b97:	55                   	push   ebp
  106b98:	56                   	push   esi
  106b99:	e8 a2 f8 ff ff       	call   106440 <strapd>
  106b9e:	83 c4 10             	add    esp,0x10
  106ba1:	0f b6 4c 24 0f       	movzx  ecx,BYTE PTR [esp+0xf]
  106ba6:	e9 5e ff ff ff       	jmp    106b09 <sprintf+0x39>
  106bab:	90                   	nop
  106bac:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
            switch (c)
  106bb0:	3c 62                	cmp    al,0x62
  106bb2:	74 7c                	je     106c30 <sprintf+0x160>
  106bb4:	7f 1a                	jg     106bd0 <sprintf+0x100>
  106bb6:	3c 25                	cmp    al,0x25
  106bb8:	75 52                	jne    106c0c <sprintf+0x13c>
                    strapd(str, '%');
  106bba:	83 ec 08             	sub    esp,0x8
  106bbd:	6a 25                	push   0x25
  106bbf:	56                   	push   esi
  106bc0:	e8 7b f8 ff ff       	call   106440 <strapd>
  106bc5:	83 c4 10             	add    esp,0x10
            wait_for_format = false;
  106bc8:	31 c9                	xor    ecx,ecx
  106bca:	eb a7                	jmp    106b73 <sprintf+0xa3>
  106bcc:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
                    strapd(str, va_arg(va, char));
  106bd0:	0f be 07             	movsx  eax,BYTE PTR [edi]
  106bd3:	83 ec 08             	sub    esp,0x8
  106bd6:	8d 6f 04             	lea    ebp,[edi+0x4]
  106bd9:	89 ef                	mov    edi,ebp
  106bdb:	50                   	push   eax
  106bdc:	56                   	push   esi
  106bdd:	e8 5e f8 ff ff       	call   106440 <strapd>
  106be2:	83 c4 10             	add    esp,0x10
            wait_for_format = false;
  106be5:	31 c9                	xor    ecx,ecx
  106be7:	eb 8a                	jmp    106b73 <sprintf+0xa3>
  106be9:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
                    itos(va_arg(va, unsigned int), temp, 10);
  106bf0:	83 ec 04             	sub    esp,0x4
  106bf3:	8d 6f 04             	lea    ebp,[edi+0x4]
  106bf6:	6a 0a                	push   0xa
  106bf8:	8d 44 24 18          	lea    eax,[esp+0x18]
  106bfc:	50                   	push   eax
  106bfd:	ff 37                	push   DWORD PTR [edi]
  106bff:	e9 56 ff ff ff       	jmp    106b5a <sprintf+0x8a>
  106c04:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
            switch (c)
  106c08:	3c 69                	cmp    al,0x69
  106c0a:	74 74                	je     106c80 <sprintf+0x1b0>
                    strapd(str, '%');
  106c0c:	83 ec 08             	sub    esp,0x8
  106c0f:	6a 25                	push   0x25
  106c11:	56                   	push   esi
  106c12:	e8 29 f8 ff ff       	call   106440 <strapd>
                    strapd(str, c);
  106c17:	58                   	pop    eax
  106c18:	5a                   	pop    edx
  106c19:	55                   	push   ebp
  106c1a:	56                   	push   esi
  106c1b:	e8 20 f8 ff ff       	call   106440 <strapd>
  106c20:	83 c4 10             	add    esp,0x10
            wait_for_format = false;
  106c23:	31 c9                	xor    ecx,ecx
  106c25:	e9 49 ff ff ff       	jmp    106b73 <sprintf+0xa3>
  106c2a:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
                    itos(va_arg(va, unsigned int), temp, 2);
  106c30:	83 ec 04             	sub    esp,0x4
  106c33:	8d 6f 04             	lea    ebp,[edi+0x4]
  106c36:	6a 02                	push   0x2
  106c38:	8d 44 24 18          	lea    eax,[esp+0x18]
  106c3c:	50                   	push   eax
  106c3d:	ff 37                	push   DWORD PTR [edi]
  106c3f:	e8 ec 02 00 00       	call   106f30 <itos>
                    strcat(str, "0b");
  106c44:	58                   	pop    eax
  106c45:	5a                   	pop    edx
  106c46:	68 91 24 11 00       	push   0x112491
  106c4b:	56                   	push   esi
  106c4c:	e8 8f ec ff ff       	call   1058e0 <strcat>
  106c51:	e9 09 ff ff ff       	jmp    106b5f <sprintf+0x8f>
  106c56:	8d 76 00             	lea    esi,[esi+0x0]
  106c59:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
                    strcat(str, va_arg(va, const char *));
  106c60:	83 ec 08             	sub    esp,0x8
  106c63:	8d 6f 04             	lea    ebp,[edi+0x4]
  106c66:	ff 37                	push   DWORD PTR [edi]
  106c68:	56                   	push   esi
  106c69:	89 ef                	mov    edi,ebp
  106c6b:	e8 70 ec ff ff       	call   1058e0 <strcat>
  106c70:	83 c4 10             	add    esp,0x10
            wait_for_format = false;
  106c73:	31 c9                	xor    ecx,ecx
  106c75:	e9 f9 fe ff ff       	jmp    106b73 <sprintf+0xa3>
  106c7a:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
                        int value = va_arg(va, unsigned int);
  106c80:	8d 6f 04             	lea    ebp,[edi+0x4]
  106c83:	8b 3f                	mov    edi,DWORD PTR [edi]
                        if (value < 0)
  106c85:	85 ff                	test   edi,edi
  106c87:	78 10                	js     106c99 <sprintf+0x1c9>
                        itos(uvalue, temp, 10);
  106c89:	83 ec 04             	sub    esp,0x4
  106c8c:	6a 0a                	push   0xa
  106c8e:	8d 44 24 18          	lea    eax,[esp+0x18]
  106c92:	50                   	push   eax
  106c93:	57                   	push   edi
  106c94:	e9 c1 fe ff ff       	jmp    106b5a <sprintf+0x8a>
                            strcat(str, "-");
  106c99:	83 ec 08             	sub    esp,0x8
                            uvalue = 0 - value;
  106c9c:	f7 df                	neg    edi
                            strcat(str, "-");
  106c9e:	68 f1 21 11 00       	push   0x1121f1
  106ca3:	56                   	push   esi
  106ca4:	e8 37 ec ff ff       	call   1058e0 <strcat>
  106ca9:	83 c4 10             	add    esp,0x10
  106cac:	eb db                	jmp    106c89 <sprintf+0x1b9>
  106cae:	66 90                	xchg   ax,ax

00106cb0 <vsprintf>:
{
  106cb0:	55                   	push   ebp
  106cb1:	57                   	push   edi
  106cb2:	56                   	push   esi
  106cb3:	53                   	push   ebx
  106cb4:	83 ec 3c             	sub    esp,0x3c
  106cb7:	8b 74 24 50          	mov    esi,DWORD PTR [esp+0x50]
  106cbb:	8b 5c 24 54          	mov    ebx,DWORD PTR [esp+0x54]
  106cbf:	8b 7c 24 58          	mov    edi,DWORD PTR [esp+0x58]
    str[0] = '\0';
  106cc3:	c6 06 00             	mov    BYTE PTR [esi],0x0
    for (unsigned int i = 0; format[i]; i++)
  106cc6:	0f b6 03             	movzx  eax,BYTE PTR [ebx]
  106cc9:	84 c0                	test   al,al
  106ccb:	0f 84 8d 00 00 00    	je     106d5e <vsprintf+0xae>
  106cd1:	83 c3 01             	add    ebx,0x1
    bool wait_for_format = false;
  106cd4:	31 c9                	xor    ecx,ecx
  106cd6:	eb 1c                	jmp    106cf4 <vsprintf+0x44>
  106cd8:	90                   	nop
  106cd9:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
        if (c == '%' && !wait_for_format)
  106ce0:	84 c9                	test   cl,cl
  106ce2:	75 14                	jne    106cf8 <vsprintf+0x48>
            wait_for_format = true;
  106ce4:	b9 01 00 00 00       	mov    ecx,0x1
  106ce9:	83 c3 01             	add    ebx,0x1
    for (unsigned int i = 0; format[i]; i++)
  106cec:	0f b6 43 ff          	movzx  eax,BYTE PTR [ebx-0x1]
  106cf0:	84 c0                	test   al,al
  106cf2:	74 6a                	je     106d5e <vsprintf+0xae>
        if (c == '%' && !wait_for_format)
  106cf4:	3c 25                	cmp    al,0x25
  106cf6:	74 e8                	je     106ce0 <vsprintf+0x30>
        else if (wait_for_format)
  106cf8:	84 c9                	test   cl,cl
  106cfa:	0f be e8             	movsx  ebp,al
  106cfd:	74 71                	je     106d70 <vsprintf+0xc0>
            switch (c)
  106cff:	3c 64                	cmp    al,0x64
  106d01:	0f 84 59 01 00 00    	je     106e60 <vsprintf+0x1b0>
  106d07:	0f 8e 83 00 00 00    	jle    106d90 <vsprintf+0xe0>
  106d0d:	3c 73                	cmp    al,0x73
  106d0f:	0f 84 2b 01 00 00    	je     106e40 <vsprintf+0x190>
  106d15:	0f 8e cd 00 00 00    	jle    106de8 <vsprintf+0x138>
  106d1b:	3c 75                	cmp    al,0x75
  106d1d:	0f 84 ad 00 00 00    	je     106dd0 <vsprintf+0x120>
  106d23:	3c 78                	cmp    al,0x78
  106d25:	0f 85 c1 00 00 00    	jne    106dec <vsprintf+0x13c>
                    itos(va_arg(va, unsigned int), temp, 16);
  106d2b:	83 ec 04             	sub    esp,0x4
  106d2e:	8d 6f 04             	lea    ebp,[edi+0x4]
  106d31:	6a 10                	push   0x10
  106d33:	8d 44 24 18          	lea    eax,[esp+0x18]
  106d37:	50                   	push   eax
  106d38:	ff 37                	push   DWORD PTR [edi]
                        itos(uvalue, temp, 10);
  106d3a:	e8 f1 01 00 00       	call   106f30 <itos>
                        strcat(str, temp);
  106d3f:	59                   	pop    ecx
  106d40:	5f                   	pop    edi
                        int value = va_arg(va, unsigned int);
  106d41:	89 ef                	mov    edi,ebp
                        strcat(str, temp);
  106d43:	8d 44 24 18          	lea    eax,[esp+0x18]
  106d47:	50                   	push   eax
  106d48:	56                   	push   esi
  106d49:	e8 92 eb ff ff       	call   1058e0 <strcat>
                    break;
  106d4e:	83 c4 10             	add    esp,0x10
            wait_for_format = false;
  106d51:	31 c9                	xor    ecx,ecx
  106d53:	83 c3 01             	add    ebx,0x1
    for (unsigned int i = 0; format[i]; i++)
  106d56:	0f b6 43 ff          	movzx  eax,BYTE PTR [ebx-0x1]
  106d5a:	84 c0                	test   al,al
  106d5c:	75 96                	jne    106cf4 <vsprintf+0x44>
    return strlen(str);
  106d5e:	83 ec 0c             	sub    esp,0xc
  106d61:	56                   	push   esi
  106d62:	e8 59 f3 ff ff       	call   1060c0 <strlen>
  106d67:	83 c4 4c             	add    esp,0x4c
  106d6a:	5b                   	pop    ebx
  106d6b:	5e                   	pop    esi
  106d6c:	5f                   	pop    edi
  106d6d:	5d                   	pop    ebp
  106d6e:	c3                   	ret    
  106d6f:	90                   	nop
  106d70:	88 4c 24 0f          	mov    BYTE PTR [esp+0xf],cl
            strapd(str, c);
  106d74:	83 ec 08             	sub    esp,0x8
  106d77:	55                   	push   ebp
  106d78:	56                   	push   esi
  106d79:	e8 c2 f6 ff ff       	call   106440 <strapd>
  106d7e:	83 c4 10             	add    esp,0x10
  106d81:	0f b6 4c 24 0f       	movzx  ecx,BYTE PTR [esp+0xf]
  106d86:	e9 5e ff ff ff       	jmp    106ce9 <vsprintf+0x39>
  106d8b:	90                   	nop
  106d8c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
            switch (c)
  106d90:	3c 62                	cmp    al,0x62
  106d92:	74 7c                	je     106e10 <vsprintf+0x160>
  106d94:	7f 1a                	jg     106db0 <vsprintf+0x100>
  106d96:	3c 25                	cmp    al,0x25
  106d98:	75 52                	jne    106dec <vsprintf+0x13c>
                    strapd(str, '%');
  106d9a:	83 ec 08             	sub    esp,0x8
  106d9d:	6a 25                	push   0x25
  106d9f:	56                   	push   esi
  106da0:	e8 9b f6 ff ff       	call   106440 <strapd>
                    break;
  106da5:	83 c4 10             	add    esp,0x10
            wait_for_format = false;
  106da8:	31 c9                	xor    ecx,ecx
  106daa:	eb a7                	jmp    106d53 <vsprintf+0xa3>
  106dac:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
                    strapd(str, va_arg(va, char));
  106db0:	0f be 07             	movsx  eax,BYTE PTR [edi]
  106db3:	83 ec 08             	sub    esp,0x8
  106db6:	8d 6f 04             	lea    ebp,[edi+0x4]
  106db9:	89 ef                	mov    edi,ebp
  106dbb:	50                   	push   eax
  106dbc:	56                   	push   esi
  106dbd:	e8 7e f6 ff ff       	call   106440 <strapd>
                    break;
  106dc2:	83 c4 10             	add    esp,0x10
            wait_for_format = false;
  106dc5:	31 c9                	xor    ecx,ecx
  106dc7:	eb 8a                	jmp    106d53 <vsprintf+0xa3>
  106dc9:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
                    itos(va_arg(va, unsigned int), temp, 10);
  106dd0:	83 ec 04             	sub    esp,0x4
  106dd3:	8d 6f 04             	lea    ebp,[edi+0x4]
  106dd6:	6a 0a                	push   0xa
  106dd8:	8d 44 24 18          	lea    eax,[esp+0x18]
  106ddc:	50                   	push   eax
  106ddd:	ff 37                	push   DWORD PTR [edi]
  106ddf:	e9 56 ff ff ff       	jmp    106d3a <vsprintf+0x8a>
  106de4:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
            switch (c)
  106de8:	3c 69                	cmp    al,0x69
  106dea:	74 74                	je     106e60 <vsprintf+0x1b0>
                    strapd(str, '%');
  106dec:	83 ec 08             	sub    esp,0x8
  106def:	6a 25                	push   0x25
  106df1:	56                   	push   esi
  106df2:	e8 49 f6 ff ff       	call   106440 <strapd>
                    strapd(str, c);
  106df7:	58                   	pop    eax
  106df8:	5a                   	pop    edx
  106df9:	55                   	push   ebp
  106dfa:	56                   	push   esi
  106dfb:	e8 40 f6 ff ff       	call   106440 <strapd>
                    break;
  106e00:	83 c4 10             	add    esp,0x10
            wait_for_format = false;
  106e03:	31 c9                	xor    ecx,ecx
  106e05:	e9 49 ff ff ff       	jmp    106d53 <vsprintf+0xa3>
  106e0a:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
                    itos(va_arg(va, unsigned int), temp, 2);
  106e10:	83 ec 04             	sub    esp,0x4
  106e13:	8d 6f 04             	lea    ebp,[edi+0x4]
  106e16:	6a 02                	push   0x2
  106e18:	8d 44 24 18          	lea    eax,[esp+0x18]
  106e1c:	50                   	push   eax
  106e1d:	ff 37                	push   DWORD PTR [edi]
  106e1f:	e8 0c 01 00 00       	call   106f30 <itos>
                    strcat(str, "0b");
  106e24:	58                   	pop    eax
  106e25:	5a                   	pop    edx
  106e26:	68 91 24 11 00       	push   0x112491
  106e2b:	56                   	push   esi
  106e2c:	e8 af ea ff ff       	call   1058e0 <strcat>
  106e31:	e9 09 ff ff ff       	jmp    106d3f <vsprintf+0x8f>
  106e36:	8d 76 00             	lea    esi,[esi+0x0]
  106e39:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
                    strcat(str, va_arg(va, const char *));
  106e40:	83 ec 08             	sub    esp,0x8
  106e43:	8d 6f 04             	lea    ebp,[edi+0x4]
  106e46:	ff 37                	push   DWORD PTR [edi]
  106e48:	56                   	push   esi
  106e49:	89 ef                	mov    edi,ebp
  106e4b:	e8 90 ea ff ff       	call   1058e0 <strcat>
                    break;
  106e50:	83 c4 10             	add    esp,0x10
            wait_for_format = false;
  106e53:	31 c9                	xor    ecx,ecx
  106e55:	e9 f9 fe ff ff       	jmp    106d53 <vsprintf+0xa3>
  106e5a:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
                        int value = va_arg(va, unsigned int);
  106e60:	8d 6f 04             	lea    ebp,[edi+0x4]
  106e63:	8b 3f                	mov    edi,DWORD PTR [edi]
                        if (value < 0)
  106e65:	85 ff                	test   edi,edi
  106e67:	78 10                	js     106e79 <vsprintf+0x1c9>
                        itos(uvalue, temp, 10);
  106e69:	83 ec 04             	sub    esp,0x4
  106e6c:	6a 0a                	push   0xa
  106e6e:	8d 44 24 18          	lea    eax,[esp+0x18]
  106e72:	50                   	push   eax
  106e73:	57                   	push   edi
  106e74:	e9 c1 fe ff ff       	jmp    106d3a <vsprintf+0x8a>
                            strcat(str, "-");
  106e79:	83 ec 08             	sub    esp,0x8
                            uvalue = 0 - value;
  106e7c:	f7 df                	neg    edi
                            strcat(str, "-");
  106e7e:	68 f1 21 11 00       	push   0x1121f1
  106e83:	56                   	push   esi
  106e84:	e8 57 ea ff ff       	call   1058e0 <strcat>
  106e89:	83 c4 10             	add    esp,0x10
  106e8c:	eb db                	jmp    106e69 <vsprintf+0x1b9>
  106e8e:	66 90                	xchg   ax,ax

00106e90 <stoi>:

const char * basechar     = "0123456789abcdefghijklmnopqrstuvwxyz";
const char *  basechar_maj = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

unsigned int stoi(const char * str, unsigned char base)
{
  106e90:	55                   	push   ebp
  106e91:	57                   	push   edi
  106e92:	56                   	push   esi
  106e93:	53                   	push   ebx
  106e94:	83 ec 0c             	sub    esp,0xc
  106e97:	8b 6c 24 20          	mov    ebp,DWORD PTR [esp+0x20]
  106e9b:	8b 44 24 24          	mov    eax,DWORD PTR [esp+0x24]
    unsigned int i = 0;
    unsigned int value = 0;

    while (str[i])
  106e9f:	0f b6 4d 00          	movzx  ecx,BYTE PTR [ebp+0x0]
{
  106ea3:	88 44 24 03          	mov    BYTE PTR [esp+0x3],al
    while (str[i])
  106ea7:	84 c9                	test   cl,cl
  106ea9:	74 72                	je     106f1d <stoi+0x8d>
  106eab:	89 c3                	mov    ebx,eax
    {
        value = value * base;

        for (unsigned char j = 0; j < base; j++)
        {
            if ((basechar[j] == str[i]) | (basechar_maj[j] == str[i]))
  106ead:	8b 3d 2c 70 11 00    	mov    edi,DWORD PTR ds:0x11702c
  106eb3:	8b 35 28 70 11 00    	mov    esi,DWORD PTR ds:0x117028
  106eb9:	8d 53 ff             	lea    edx,[ebx-0x1]
  106ebc:	0f b6 c0             	movzx  eax,al
  106ebf:	83 c5 01             	add    ebp,0x1
  106ec2:	89 44 24 04          	mov    DWORD PTR [esp+0x4],eax
    unsigned int value = 0;
  106ec6:	31 c0                	xor    eax,eax
  106ec8:	0f b6 da             	movzx  ebx,dl
  106ecb:	89 5c 24 08          	mov    DWORD PTR [esp+0x8],ebx
  106ecf:	90                   	nop
        value = value * base;
  106ed0:	0f af 44 24 04       	imul   eax,DWORD PTR [esp+0x4]
        for (unsigned char j = 0; j < base; j++)
  106ed5:	80 7c 24 03 00       	cmp    BYTE PTR [esp+0x3],0x0
  106eda:	74 2e                	je     106f0a <stoi+0x7a>
  106edc:	8b 5c 24 08          	mov    ebx,DWORD PTR [esp+0x8]
  106ee0:	31 d2                	xor    edx,edx
  106ee2:	83 c3 01             	add    ebx,0x1
  106ee5:	eb 15                	jmp    106efc <stoi+0x6c>
  106ee7:	89 f6                	mov    esi,esi
  106ee9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
            if ((basechar[j] == str[i]) | (basechar_maj[j] == str[i]))
  106ef0:	38 0c 17             	cmp    BYTE PTR [edi+edx*1],cl
  106ef3:	74 0c                	je     106f01 <stoi+0x71>
  106ef5:	83 c2 01             	add    edx,0x1
        for (unsigned char j = 0; j < base; j++)
  106ef8:	39 d3                	cmp    ebx,edx
  106efa:	74 0e                	je     106f0a <stoi+0x7a>
            if ((basechar[j] == str[i]) | (basechar_maj[j] == str[i]))
  106efc:	38 0c 16             	cmp    BYTE PTR [esi+edx*1],cl
  106eff:	75 ef                	jne    106ef0 <stoi+0x60>
            {
                value += j;
  106f01:	01 d0                	add    eax,edx
  106f03:	83 c2 01             	add    edx,0x1
        for (unsigned char j = 0; j < base; j++)
  106f06:	39 d3                	cmp    ebx,edx
  106f08:	75 f2                	jne    106efc <stoi+0x6c>
  106f0a:	83 c5 01             	add    ebp,0x1
    while (str[i])
  106f0d:	0f b6 4d ff          	movzx  ecx,BYTE PTR [ebp-0x1]
  106f11:	84 c9                	test   cl,cl
  106f13:	75 bb                	jne    106ed0 <stoi+0x40>

        i++;
    }

    return value;
}
  106f15:	83 c4 0c             	add    esp,0xc
  106f18:	5b                   	pop    ebx
  106f19:	5e                   	pop    esi
  106f1a:	5f                   	pop    edi
  106f1b:	5d                   	pop    ebp
  106f1c:	c3                   	ret    
  106f1d:	83 c4 0c             	add    esp,0xc
    unsigned int value = 0;
  106f20:	31 c0                	xor    eax,eax
}
  106f22:	5b                   	pop    ebx
  106f23:	5e                   	pop    esi
  106f24:	5f                   	pop    edi
  106f25:	5d                   	pop    ebp
  106f26:	c3                   	ret    
  106f27:	89 f6                	mov    esi,esi
  106f29:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00106f30 <itos>:

void itos(unsigned int value, char * buffer, unsigned char base)
{
  106f30:	57                   	push   edi
  106f31:	56                   	push   esi
  106f32:	53                   	push   ebx
  106f33:	8b 5c 24 10          	mov    ebx,DWORD PTR [esp+0x10]
  106f37:	8b 7c 24 14          	mov    edi,DWORD PTR [esp+0x14]
  106f3b:	0f b6 74 24 18       	movzx  esi,BYTE PTR [esp+0x18]
    buffer[0] = '\0';   
    if (value == 0){ strapd(buffer, '0'); return; }
  106f40:	85 db                	test   ebx,ebx
    buffer[0] = '\0';   
  106f42:	c6 07 00             	mov    BYTE PTR [edi],0x0
    if (value == 0){ strapd(buffer, '0'); return; }
  106f45:	74 39                	je     106f80 <itos+0x50>
  106f47:	89 f6                	mov    esi,esi
  106f49:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
  106f50:	89 d8                	mov    eax,ebx
  106f52:	31 d2                	xor    edx,edx
    
    unsigned int v = value;

    while (v != 0)
    {
        strapd(buffer, basechar_maj[v % base]);
  106f54:	83 ec 08             	sub    esp,0x8
  106f57:	f7 f6                	div    esi
  106f59:	89 c3                	mov    ebx,eax
  106f5b:	a1 28 70 11 00       	mov    eax,ds:0x117028
  106f60:	0f be 04 10          	movsx  eax,BYTE PTR [eax+edx*1]
  106f64:	50                   	push   eax
  106f65:	57                   	push   edi
  106f66:	e8 d5 f4 ff ff       	call   106440 <strapd>
    while (v != 0)
  106f6b:	83 c4 10             	add    esp,0x10
  106f6e:	85 db                	test   ebx,ebx
  106f70:	75 de                	jne    106f50 <itos+0x20>
        v /= base;
    }

    strrvs(buffer);
  106f72:	89 7c 24 10          	mov    DWORD PTR [esp+0x10],edi
  106f76:	5b                   	pop    ebx
  106f77:	5e                   	pop    esi
  106f78:	5f                   	pop    edi
    strrvs(buffer);
  106f79:	e9 22 f5 ff ff       	jmp    1064a0 <strrvs>
  106f7e:	66 90                	xchg   ax,ax
    if (value == 0){ strapd(buffer, '0'); return; }
  106f80:	c7 44 24 14 30 00 00 	mov    DWORD PTR [esp+0x14],0x30
  106f87:	00 
  106f88:	89 7c 24 10          	mov    DWORD PTR [esp+0x10],edi
  106f8c:	5b                   	pop    ebx
  106f8d:	5e                   	pop    esi
  106f8e:	5f                   	pop    edi
    if (value == 0){ strapd(buffer, '0'); return; }
  106f8f:	e9 ac f4 ff ff       	jmp    106440 <strapd>
  106f94:	66 90                	xchg   ax,ax
  106f96:	66 90                	xchg   ax,ax
  106f98:	66 90                	xchg   ax,ax
  106f9a:	66 90                	xchg   ax,ax
  106f9c:	66 90                	xchg   ax,ax
  106f9e:	66 90                	xchg   ax,ax

00106fa0 <malloc>:

	


void *PREFIX(malloc)(size_t req_size)
{
  106fa0:	55                   	push   ebp
  106fa1:	57                   	push   edi
  106fa2:	56                   	push   esi
  106fa3:	53                   	push   ebx
  106fa4:	83 ec 3c             	sub    esp,0x3c
  106fa7:	8b 74 24 50          	mov    esi,DWORD PTR [esp+0x50]
  106fab:	eb 1b                	jmp    106fc8 <malloc+0x28>
  106fad:	8d 76 00             	lea    esi,[esi+0x0]
	
	liballoc_lock();

	if ( size == 0 )
	{
		l_warningCount += 1;
  106fb0:	83 05 40 ee 23 00 01 	add    DWORD PTR ds:0x23ee40,0x1
		debug( "liballoc: WARNING: alloc( 0 ) called from 0x%x\n",
							__builtin_return_address(0) );
		FLUSH();
		#endif
		liballoc_unlock();
		return PREFIX(malloc)(1);
  106fb7:	be 01 00 00 00       	mov    esi,0x1
		l_warningCount += 1;
  106fbc:	83 15 44 ee 23 00 00 	adc    DWORD PTR ds:0x23ee44,0x0
		liballoc_unlock();
  106fc3:	e8 08 09 00 00       	call   1078d0 <liballoc_unlock>
		size += ALIGNMENT + ALIGN_INFO;
  106fc8:	8d 5e 20             	lea    ebx,[esi+0x20]
	liballoc_lock();
  106fcb:	e8 f0 08 00 00       	call   1078c0 <liballoc_lock>
	if ( size == 0 )
  106fd0:	85 db                	test   ebx,ebx
  106fd2:	74 dc                	je     106fb0 <malloc+0x10>
	}
	

	if ( l_memRoot == NULL )
  106fd4:	a1 5c ee 23 00       	mov    eax,ds:0x23ee5c
  106fd9:	89 5c 24 10          	mov    DWORD PTR [esp+0x10],ebx
  106fdd:	8d 7e 50             	lea    edi,[esi+0x50]
  106fe0:	89 74 24 50          	mov    DWORD PTR [esp+0x50],esi
  106fe4:	85 c0                	test   eax,eax
  106fe6:	0f 84 74 03 00 00    	je     107360 <malloc+0x3c0>

	maj = l_memRoot;
	startedBet = 0;
	
	// Start at the best bet....
	if ( l_bestBet != NULL )
  106fec:	8b 1d 58 ee 23 00    	mov    ebx,DWORD PTR ds:0x23ee58
  106ff2:	8b 4c 24 50          	mov    ecx,DWORD PTR [esp+0x50]
  106ff6:	85 db                	test   ebx,ebx
  106ff8:	8d 71 38             	lea    esi,[ecx+0x38]
  106ffb:	0f 84 f7 02 00 00    	je     1072f8 <malloc+0x358>
	{
		bestSize = l_bestBet->size - l_bestBet->usage;
  107001:	8b 53 10             	mov    edx,DWORD PTR [ebx+0x10]
  107004:	8b 4b 0c             	mov    ecx,DWORD PTR [ebx+0xc]
  107007:	c7 44 24 04 00 00 00 	mov    DWORD PTR [esp+0x4],0x0
  10700e:	00 
  10700f:	89 d5                	mov    ebp,edx
  107011:	89 54 24 08          	mov    DWORD PTR [esp+0x8],edx
  107015:	89 ca                	mov    edx,ecx
  107017:	29 ea                	sub    edx,ebp

		if ( bestSize > (size + sizeof(struct liballoc_minor)))
  107019:	39 f2                	cmp    edx,esi
		bestSize = l_bestBet->size - l_bestBet->usage;
  10701b:	89 14 24             	mov    DWORD PTR [esp],edx
		if ( bestSize > (size + sizeof(struct liballoc_minor)))
  10701e:	0f 86 54 01 00 00    	jbe    107178 <malloc+0x1d8>
		{
			maj = l_bestBet;
			startedBet = 1;
  107024:	c7 44 24 0c 01 00 00 	mov    DWORD PTR [esp+0xc],0x1
  10702b:	00 
			st  = st / (l_pageSize) + 1;
  10702c:	89 fa                	mov    edx,edi
		l_allocated += maj->size;
  10702e:	c7 44 24 2c 00 00 00 	mov    DWORD PTR [esp+0x2c],0x0
  107035:	00 
			st  = st / (l_pageSize) + 1;
  107036:	c1 ea 0c             	shr    edx,0xc
  107039:	81 e7 ff 0f 00 00    	and    edi,0xfff
  10703f:	8d 42 01             	lea    eax,[edx+0x1]
  107042:	89 54 24 14          	mov    DWORD PTR [esp+0x14],edx
  107046:	89 7c 24 18          	mov    DWORD PTR [esp+0x18],edi
		}
	}
	
	while ( maj != NULL )
	{
		diff  = maj->size - maj->usage;	
  10704a:	89 cf                	mov    edi,ecx
			st  = st / (l_pageSize) + 1;
  10704c:	89 44 24 1c          	mov    DWORD PTR [esp+0x1c],eax
  107050:	0f 44 c2             	cmove  eax,edx
  107053:	ba 10 00 00 00       	mov    edx,0x10
  107058:	83 f8 10             	cmp    eax,0x10
  10705b:	0f 43 d0             	cmovae edx,eax
		diff  = maj->size - maj->usage;	
  10705e:	2b 7c 24 08          	sub    edi,DWORD PTR [esp+0x8]
  107062:	89 54 24 20          	mov    DWORD PTR [esp+0x20],edx
		maj->size 	= st * l_pageSize;
  107066:	c1 e2 0c             	shl    edx,0xc
  107069:	89 54 24 24          	mov    DWORD PTR [esp+0x24],edx
		l_allocated += maj->size;
  10706d:	89 54 24 28          	mov    DWORD PTR [esp+0x28],edx
										// free memory in the block

		if ( bestSize < diff )
  107071:	31 d2                	xor    edx,edx
  107073:	3b 54 24 04          	cmp    edx,DWORD PTR [esp+0x4]
  107077:	89 f8                	mov    eax,edi
  107079:	72 14                	jb     10708f <malloc+0xef>
  10707b:	77 05                	ja     107082 <malloc+0xe2>
  10707d:	3b 3c 24             	cmp    edi,DWORD PTR [esp]
  107080:	76 0d                	jbe    10708f <malloc+0xef>
		{
			// Hmm.. this one has more memory then our bestBet. Remember!
			l_bestBet = maj;
  107082:	89 1d 58 ee 23 00    	mov    DWORD PTR ds:0x23ee58,ebx
  107088:	89 04 24             	mov    DWORD PTR [esp],eax
  10708b:	89 54 24 04          	mov    DWORD PTR [esp+0x4],edx
		
		
#ifdef USE_CASE1
			
		// CASE 1:  There is not enough space in this major block.
		if ( diff < (size + sizeof( struct liballoc_minor )) )
  10708f:	39 f7                	cmp    edi,esi
  107091:	73 2b                	jae    1070be <malloc+0x11e>
			debug( "CASE 1: Insufficient space in block 0x%x\n", maj);
			FLUSH();
			#endif
				
				// Another major block next to this one?
			if ( maj->next != NULL ) 
  107093:	8b 43 04             	mov    eax,DWORD PTR [ebx+0x4]
  107096:	85 c0                	test   eax,eax
  107098:	0f 84 1a 01 00 00    	je     1071b8 <malloc+0x218>
  10709e:	8b 58 10             	mov    ebx,DWORD PTR [eax+0x10]
  1070a1:	8b 48 0c             	mov    ecx,DWORD PTR [eax+0xc]
		if ( bestSize < diff )
  1070a4:	31 d2                	xor    edx,edx
  1070a6:	89 5c 24 08          	mov    DWORD PTR [esp+0x8],ebx
		diff  = maj->size - maj->usage;	
  1070aa:	89 cf                	mov    edi,ecx
  1070ac:	2b 7c 24 08          	sub    edi,DWORD PTR [esp+0x8]
		if ( bestSize < diff )
  1070b0:	3b 54 24 04          	cmp    edx,DWORD PTR [esp+0x4]
			}

			if ( startedBet == 1 )		// If we started at the best bet,
			{							// let's start all over again.
				maj = l_memRoot;
				startedBet = 0;
  1070b4:	89 c3                	mov    ebx,eax
		if ( bestSize < diff )
  1070b6:	89 f8                	mov    eax,edi
  1070b8:	73 c1                	jae    10707b <malloc+0xdb>
		if ( diff < (size + sizeof( struct liballoc_minor )) )
  1070ba:	39 f7                	cmp    edi,esi
  1070bc:	72 d5                	jb     107093 <malloc+0xf3>
#endif

#ifdef USE_CASE2
		
		// CASE 2: It's a brand new block.
		if ( maj->first == NULL )
  1070be:	8b 53 14             	mov    edx,DWORD PTR [ebx+0x14]
  1070c1:	85 d2                	test   edx,edx
  1070c3:	0f 84 9e 03 00 00    	je     107467 <malloc+0x4c7>
				
#ifdef USE_CASE3

		// CASE 3: Block in use and enough space at the start of the block.
		diff =  (uintptr_t)(maj->first);
		diff -= (uintptr_t)maj;
  1070c9:	89 d0                	mov    eax,edx
  1070cb:	29 d8                	sub    eax,ebx
		diff -= sizeof(struct liballoc_major);
  1070cd:	83 e8 18             	sub    eax,0x18

		if ( diff >= (size + sizeof(struct liballoc_minor)) )
  1070d0:	39 f0                	cmp    eax,esi
  1070d2:	0f 83 19 03 00 00    	jae    1073f1 <malloc+0x451>
		
			// Looping within the block now...
		while ( min != NULL )
		{
				// CASE 4.1: End of minors in a block. Space from last and end?
				if ( min->next == NULL )
  1070d8:	8b 7a 04             	mov    edi,DWORD PTR [edx+0x4]
  1070db:	8b 6a 10             	mov    ebp,DWORD PTR [edx+0x10]
  1070de:	85 ff                	test   edi,edi
  1070e0:	74 1d                	je     1070ff <malloc+0x15f>
				// CASE 4.2: Is there space between two minors?
				if ( min->next != NULL )
				{
					// is the difference between here and next big enough?
					diff  = (uintptr_t)(min->next);
					diff -= (uintptr_t)min;
  1070e2:	89 f8                	mov    eax,edi
  1070e4:	29 d0                	sub    eax,edx
					diff -= sizeof( struct liballoc_minor );
  1070e6:	83 e8 18             	sub    eax,0x18
					diff -= min->size;
  1070e9:	29 e8                	sub    eax,ebp
										// minus our existing usage.

					if ( diff >= (size + sizeof( struct liballoc_minor )) )
  1070eb:	39 f0                	cmp    eax,esi
  1070ed:	0f 83 ad 01 00 00    	jae    1072a0 <malloc+0x300>
  1070f3:	89 fa                	mov    edx,edi
				if ( min->next == NULL )
  1070f5:	8b 7a 04             	mov    edi,DWORD PTR [edx+0x4]
  1070f8:	8b 6a 10             	mov    ebp,DWORD PTR [edx+0x10]
  1070fb:	85 ff                	test   edi,edi
  1070fd:	75 e3                	jne    1070e2 <malloc+0x142>
					diff -= sizeof( struct liballoc_minor );
  1070ff:	8d 44 0b e8          	lea    eax,[ebx+ecx*1-0x18]
  107103:	29 d0                	sub    eax,edx
					diff -= min->size; 
  107105:	29 e8                	sub    eax,ebp
					if ( diff >= (size + sizeof( struct liballoc_minor )) )
  107107:	39 f0                	cmp    eax,esi
  107109:	0f 83 11 02 00 00    	jae    107320 <malloc+0x380>
#endif

#ifdef USE_CASE5

		// CASE 5: Block full! Ensure next block and loop.
		if ( maj->next == NULL ) 
  10710f:	8b 43 04             	mov    eax,DWORD PTR [ebx+0x4]
  107112:	85 c0                	test   eax,eax
  107114:	75 88                	jne    10709e <malloc+0xfe>
			#ifdef DEBUG
			debug( "CASE 5: block full\n");
			FLUSH();
			#endif

			if ( startedBet == 1 )
  107116:	83 7c 24 0c 01       	cmp    DWORD PTR [esp+0xc],0x1
  10711b:	74 7b                	je     107198 <malloc+0x1f8>
		maj = (struct liballoc_major*)liballoc_alloc( st );
  10711d:	83 ec 0c             	sub    esp,0xc
  107120:	ff 74 24 2c          	push   DWORD PTR [esp+0x2c]
  107124:	e8 b7 07 00 00       	call   1078e0 <liballoc_alloc>
		if ( maj == NULL ) 
  107129:	83 c4 10             	add    esp,0x10
  10712c:	85 c0                	test   eax,eax
  10712e:	0f 84 9c 02 00 00    	je     1073d0 <malloc+0x430>
		maj->pages 	= st;
  107134:	8b 4c 24 20          	mov    ecx,DWORD PTR [esp+0x20]
		l_allocated += maj->size;
  107138:	8b 7c 24 28          	mov    edi,DWORD PTR [esp+0x28]
  10713c:	01 3d 50 ee 23 00    	add    DWORD PTR ds:0x23ee50,edi
		maj->next 	= NULL;
  107142:	c7 40 04 00 00 00 00 	mov    DWORD PTR [eax+0x4],0x0
		maj->usage 	= sizeof(struct liballoc_major);
  107149:	c7 40 10 18 00 00 00 	mov    DWORD PTR [eax+0x10],0x18
		maj->first 	= NULL;
  107150:	c7 40 14 00 00 00 00 	mov    DWORD PTR [eax+0x14],0x0
		maj->pages 	= st;
  107157:	89 48 08             	mov    DWORD PTR [eax+0x8],ecx
		maj->size 	= st * l_pageSize;
  10715a:	8b 4c 24 24          	mov    ecx,DWORD PTR [esp+0x24]
		l_allocated += maj->size;
  10715e:	8b 6c 24 2c          	mov    ebp,DWORD PTR [esp+0x2c]
  107162:	11 2d 54 ee 23 00    	adc    DWORD PTR ds:0x23ee54,ebp
		maj->size 	= st * l_pageSize;
  107168:	89 48 0c             	mov    DWORD PTR [eax+0xc],ecx
				startedBet = 0;
				continue;
			}
				
			// we've run out. we need more...
			maj->next = allocate_new_page( size );		// next one guaranteed to be okay
  10716b:	89 43 04             	mov    DWORD PTR [ebx+0x4],eax
			if ( maj->next == NULL ) break;			//  uh oh,  no more memory.....
			maj->next->prev = maj;
  10716e:	89 18                	mov    DWORD PTR [eax],ebx
  107170:	e9 29 ff ff ff       	jmp    10709e <malloc+0xfe>
  107175:	8d 76 00             	lea    esi,[esi+0x0]
  107178:	8b 58 10             	mov    ebx,DWORD PTR [eax+0x10]
  10717b:	8b 48 0c             	mov    ecx,DWORD PTR [eax+0xc]
	startedBet = 0;
  10717e:	c7 44 24 0c 00 00 00 	mov    DWORD PTR [esp+0xc],0x0
  107185:	00 
  107186:	89 5c 24 08          	mov    DWORD PTR [esp+0x8],ebx
		if ( bestSize > (size + sizeof(struct liballoc_minor)))
  10718a:	89 c3                	mov    ebx,eax
  10718c:	e9 9b fe ff ff       	jmp    10702c <malloc+0x8c>
  107191:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
				maj = l_memRoot;
  107198:	a1 5c ee 23 00       	mov    eax,ds:0x23ee5c
	while ( maj != NULL )
  10719d:	85 c0                	test   eax,eax
  10719f:	0f 84 40 02 00 00    	je     1073e5 <malloc+0x445>
				startedBet = 0;
  1071a5:	c7 44 24 0c 00 00 00 	mov    DWORD PTR [esp+0xc],0x0
  1071ac:	00 
  1071ad:	e9 ec fe ff ff       	jmp    10709e <malloc+0xfe>
  1071b2:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
			if ( startedBet == 1 )		// If we started at the best bet,
  1071b8:	83 7c 24 0c 01       	cmp    DWORD PTR [esp+0xc],0x1
  1071bd:	74 d9                	je     107198 <malloc+0x1f8>
			st  = st / (l_pageSize) + 1;
  1071bf:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
  1071c3:	bf 10 00 00 00       	mov    edi,0x10
  1071c8:	85 c0                	test   eax,eax
  1071ca:	8b 44 24 1c          	mov    eax,DWORD PTR [esp+0x1c]
  1071ce:	0f 44 44 24 14       	cmove  eax,DWORD PTR [esp+0x14]
  1071d3:	83 f8 10             	cmp    eax,0x10
  1071d6:	0f 43 f8             	cmovae edi,eax
		maj = (struct liballoc_major*)liballoc_alloc( st );
  1071d9:	83 ec 0c             	sub    esp,0xc
  1071dc:	57                   	push   edi
  1071dd:	e8 fe 06 00 00       	call   1078e0 <liballoc_alloc>
		if ( maj == NULL ) 
  1071e2:	83 c4 10             	add    esp,0x10
  1071e5:	85 c0                	test   eax,eax
  1071e7:	0f 84 e3 01 00 00    	je     1073d0 <malloc+0x430>
		maj->pages 	= st;
  1071ed:	89 78 08             	mov    DWORD PTR [eax+0x8],edi
		l_allocated += maj->size;
  1071f0:	31 ed                	xor    ebp,ebp
		maj->size 	= st * l_pageSize;
  1071f2:	c1 e7 0c             	shl    edi,0xc
		l_allocated += maj->size;
  1071f5:	01 3d 50 ee 23 00    	add    DWORD PTR ds:0x23ee50,edi
		maj->next 	= NULL;
  1071fb:	c7 40 04 00 00 00 00 	mov    DWORD PTR [eax+0x4],0x0
		l_allocated += maj->size;
  107202:	11 2d 54 ee 23 00    	adc    DWORD PTR ds:0x23ee54,ebp
		maj->size 	= st * l_pageSize;
  107208:	89 78 0c             	mov    DWORD PTR [eax+0xc],edi
		maj->usage 	= sizeof(struct liballoc_major);
  10720b:	c7 40 10 18 00 00 00 	mov    DWORD PTR [eax+0x10],0x18
		maj->first 	= NULL;
  107212:	c7 40 14 00 00 00 00 	mov    DWORD PTR [eax+0x14],0x0
			maj->next->prev = maj;
  107219:	c7 44 24 08 18 00 00 	mov    DWORD PTR [esp+0x8],0x18
  107220:	00 
			maj->next = allocate_new_page( size );	// next one will be okay.
  107221:	89 43 04             	mov    DWORD PTR [ebx+0x4],eax
			maj->next->prev = maj;
  107224:	89 18                	mov    DWORD PTR [eax],ebx
			maj->first->req_size 	= req_size;
  107226:	8b 5c 24 50          	mov    ebx,DWORD PTR [esp+0x50]
			maj->first = (struct liballoc_minor*)((uintptr_t)maj + sizeof(struct liballoc_major) );
  10722a:	8d 50 18             	lea    edx,[eax+0x18]
			maj->usage 	+= size + sizeof( struct liballoc_minor );
  10722d:	03 74 24 08          	add    esi,DWORD PTR [esp+0x8]
			ALIGN( p );
  107231:	8d 68 40             	lea    ebp,[eax+0x40]
			maj->first->size 		= size;
  107234:	8b 4c 24 10          	mov    ecx,DWORD PTR [esp+0x10]
			maj->first->magic 		= LIBALLOC_MAGIC;
  107238:	c7 40 24 de c0 01 c0 	mov    DWORD PTR [eax+0x24],0xc001c0de
			maj->first = (struct liballoc_minor*)((uintptr_t)maj + sizeof(struct liballoc_major) );
  10723f:	89 50 14             	mov    DWORD PTR [eax+0x14],edx
			maj->first->prev 		= NULL;
  107242:	c7 40 18 00 00 00 00 	mov    DWORD PTR [eax+0x18],0x0
			maj->first->req_size 	= req_size;
  107249:	89 58 2c             	mov    DWORD PTR [eax+0x2c],ebx
			ALIGN( p );
  10724c:	89 ea                	mov    edx,ebp
			l_inuse += size;
  10724e:	31 db                	xor    ebx,ebx
  107250:	01 0d 48 ee 23 00    	add    DWORD PTR ds:0x23ee48,ecx
			maj->first->next 		= NULL;
  107256:	c7 40 1c 00 00 00 00 	mov    DWORD PTR [eax+0x1c],0x0
			l_inuse += size;
  10725d:	11 1d 4c ee 23 00    	adc    DWORD PTR ds:0x23ee4c,ebx
			ALIGN( p );
  107263:	83 e2 0f             	and    edx,0xf
			maj->first->block 		= maj;
  107266:	89 40 20             	mov    DWORD PTR [eax+0x20],eax
			maj->first->size 		= size;
  107269:	89 48 28             	mov    DWORD PTR [eax+0x28],ecx
			maj->usage 	+= size + sizeof( struct liballoc_minor );
  10726c:	89 70 10             	mov    DWORD PTR [eax+0x10],esi
			ALIGN( p );
  10726f:	74 7b                	je     1072ec <malloc+0x34c>
  107271:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
						ALIGN( p );
  107278:	b8 10 00 00 00       	mov    eax,0x10
  10727d:	29 d0                	sub    eax,edx
  10727f:	01 c5                	add    ebp,eax
  107281:	83 c0 10             	add    eax,0x10
  107284:	88 45 f0             	mov    BYTE PTR [ebp-0x10],al
						liballoc_unlock();		// release the lock
  107287:	e8 44 06 00 00       	call   1078d0 <liballoc_unlock>
	debug( "liballoc: WARNING: PREFIX(malloc)( %i ) returning NULL.\n", size);
	liballoc_dump();
	FLUSH();
	#endif
	return NULL;
}
  10728c:	83 c4 3c             	add    esp,0x3c
  10728f:	89 e8                	mov    eax,ebp
  107291:	5b                   	pop    ebx
  107292:	5e                   	pop    esi
  107293:	5f                   	pop    edi
  107294:	5d                   	pop    ebp
  107295:	c3                   	ret    
  107296:	8d 76 00             	lea    esi,[esi+0x0]
  107299:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
						new_min->req_size = req_size;
  1072a0:	8b 4c 24 50          	mov    ecx,DWORD PTR [esp+0x50]
						new_min = (struct liballoc_minor*)((uintptr_t)min + sizeof( struct liballoc_minor ) + min->size);
  1072a4:	01 d5                	add    ebp,edx
						maj->usage += size + sizeof( struct liballoc_minor );
  1072a6:	03 74 24 08          	add    esi,DWORD PTR [esp+0x8]
						new_min->next = min->next;
  1072aa:	89 7d 1c             	mov    DWORD PTR [ebp+0x1c],edi
						new_min->size = size;
  1072ad:	8b 7c 24 10          	mov    edi,DWORD PTR [esp+0x10]
						new_min = (struct liballoc_minor*)((uintptr_t)min + sizeof( struct liballoc_minor ) + min->size);
  1072b1:	8d 45 18             	lea    eax,[ebp+0x18]
						new_min->magic = LIBALLOC_MAGIC;
  1072b4:	c7 45 24 de c0 01 c0 	mov    DWORD PTR [ebp+0x24],0xc001c0de
						new_min->prev = min;
  1072bb:	89 55 18             	mov    DWORD PTR [ebp+0x18],edx
						new_min->req_size = req_size;
  1072be:	89 4d 2c             	mov    DWORD PTR [ebp+0x2c],ecx
						min->next->prev = new_min;
  1072c1:	8b 4a 04             	mov    ecx,DWORD PTR [edx+0x4]
						new_min->size = size;
  1072c4:	89 7d 28             	mov    DWORD PTR [ebp+0x28],edi
						new_min->block = maj;
  1072c7:	89 5d 20             	mov    DWORD PTR [ebp+0x20],ebx
						min->next->prev = new_min;
  1072ca:	89 01                	mov    DWORD PTR [ecx],eax
						min->next = new_min;
  1072cc:	89 42 04             	mov    DWORD PTR [edx+0x4],eax
						l_inuse += size;
  1072cf:	89 f8                	mov    eax,edi
						maj->usage += size + sizeof( struct liballoc_minor );
  1072d1:	89 73 10             	mov    DWORD PTR [ebx+0x10],esi
						l_inuse += size;
  1072d4:	31 d2                	xor    edx,edx
  1072d6:	01 05 48 ee 23 00    	add    DWORD PTR ds:0x23ee48,eax
  1072dc:	11 15 4c ee 23 00    	adc    DWORD PTR ds:0x23ee4c,edx
						ALIGN( p );
  1072e2:	83 c5 40             	add    ebp,0x40
  1072e5:	89 ea                	mov    edx,ebp
  1072e7:	83 e2 0f             	and    edx,0xf
  1072ea:	75 8c                	jne    107278 <malloc+0x2d8>
  1072ec:	b8 10 00 00 00       	mov    eax,0x10
  1072f1:	eb 91                	jmp    107284 <malloc+0x2e4>
  1072f3:	90                   	nop
  1072f4:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
  1072f8:	8b 58 10             	mov    ebx,DWORD PTR [eax+0x10]
  1072fb:	8b 48 0c             	mov    ecx,DWORD PTR [eax+0xc]
	unsigned long long bestSize = 0;
  1072fe:	c7 04 24 00 00 00 00 	mov    DWORD PTR [esp],0x0
  107305:	c7 44 24 04 00 00 00 	mov    DWORD PTR [esp+0x4],0x0
  10730c:	00 
	startedBet = 0;
  10730d:	c7 44 24 0c 00 00 00 	mov    DWORD PTR [esp+0xc],0x0
  107314:	00 
  107315:	89 5c 24 08          	mov    DWORD PTR [esp+0x8],ebx
	if ( l_bestBet != NULL )
  107319:	89 c3                	mov    ebx,eax
  10731b:	e9 0c fd ff ff       	jmp    10702c <malloc+0x8c>
						maj->usage += size + sizeof( struct liballoc_minor );
  107320:	03 74 24 08          	add    esi,DWORD PTR [esp+0x8]
						min->next = (struct liballoc_minor*)((uintptr_t)min + sizeof( struct liballoc_minor ) + min->size);
  107324:	01 d5                	add    ebp,edx
						min->size = size;
  107326:	8b 4c 24 10          	mov    ecx,DWORD PTR [esp+0x10]
						min->req_size = req_size;
  10732a:	8b 7c 24 50          	mov    edi,DWORD PTR [esp+0x50]
						min->next = (struct liballoc_minor*)((uintptr_t)min + sizeof( struct liballoc_minor ) + min->size);
  10732e:	8d 45 18             	lea    eax,[ebp+0x18]
  107331:	89 42 04             	mov    DWORD PTR [edx+0x4],eax
						min->next->prev = min;
  107334:	89 55 18             	mov    DWORD PTR [ebp+0x18],edx
						l_inuse += size;
  107337:	89 c8                	mov    eax,ecx
						min->next = NULL;
  107339:	c7 45 1c 00 00 00 00 	mov    DWORD PTR [ebp+0x1c],0x0
						min->magic = LIBALLOC_MAGIC;
  107340:	c7 45 24 de c0 01 c0 	mov    DWORD PTR [ebp+0x24],0xc001c0de
						min->block = maj;
  107347:	89 5d 20             	mov    DWORD PTR [ebp+0x20],ebx
						min->size = size;
  10734a:	89 4d 28             	mov    DWORD PTR [ebp+0x28],ecx
						min->req_size = req_size;
  10734d:	89 7d 2c             	mov    DWORD PTR [ebp+0x2c],edi
						maj->usage += size + sizeof( struct liballoc_minor );
  107350:	89 73 10             	mov    DWORD PTR [ebx+0x10],esi
  107353:	e9 7c ff ff ff       	jmp    1072d4 <malloc+0x334>
  107358:	90                   	nop
  107359:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
		if ( (st % l_pageSize) == 0 )
  107360:	89 fa                	mov    edx,edi
  107362:	89 f8                	mov    eax,edi
  107364:	bb 10 00 00 00       	mov    ebx,0x10
  107369:	81 e2 ff 0f 00 00    	and    edx,0xfff
  10736f:	c1 e8 0c             	shr    eax,0xc
			st  = st / (l_pageSize) + 1;
  107372:	83 fa 01             	cmp    edx,0x1
  107375:	83 d8 ff             	sbb    eax,0xffffffff
  107378:	83 f8 10             	cmp    eax,0x10
  10737b:	0f 43 d8             	cmovae ebx,eax
		maj = (struct liballoc_major*)liballoc_alloc( st );
  10737e:	83 ec 0c             	sub    esp,0xc
  107381:	53                   	push   ebx
  107382:	e8 59 05 00 00       	call   1078e0 <liballoc_alloc>
		if ( maj == NULL ) 
  107387:	83 c4 10             	add    esp,0x10
  10738a:	85 c0                	test   eax,eax
  10738c:	0f 84 b1 00 00 00    	je     107443 <malloc+0x4a3>
		maj->pages 	= st;
  107392:	89 58 08             	mov    DWORD PTR [eax+0x8],ebx
		maj->size 	= st * l_pageSize;
  107395:	c1 e3 0c             	shl    ebx,0xc
		maj->prev 	= NULL;
  107398:	c7 00 00 00 00 00    	mov    DWORD PTR [eax],0x0
		l_allocated += maj->size;
  10739e:	89 d9                	mov    ecx,ebx
		maj->size 	= st * l_pageSize;
  1073a0:	89 58 0c             	mov    DWORD PTR [eax+0xc],ebx
		l_allocated += maj->size;
  1073a3:	31 db                	xor    ebx,ebx
  1073a5:	01 0d 50 ee 23 00    	add    DWORD PTR ds:0x23ee50,ecx
		maj->next 	= NULL;
  1073ab:	c7 40 04 00 00 00 00 	mov    DWORD PTR [eax+0x4],0x0
		maj->usage 	= sizeof(struct liballoc_major);
  1073b2:	c7 40 10 18 00 00 00 	mov    DWORD PTR [eax+0x10],0x18
		maj->first 	= NULL;
  1073b9:	c7 40 14 00 00 00 00 	mov    DWORD PTR [eax+0x14],0x0
		l_allocated += maj->size;
  1073c0:	11 1d 54 ee 23 00    	adc    DWORD PTR ds:0x23ee54,ebx
		l_memRoot = allocate_new_page( size );
  1073c6:	a3 5c ee 23 00       	mov    ds:0x23ee5c,eax
  1073cb:	e9 1c fc ff ff       	jmp    106fec <malloc+0x4c>
			l_warningCount += 1;
  1073d0:	83 05 40 ee 23 00 01 	add    DWORD PTR ds:0x23ee40,0x1
			maj->next = allocate_new_page( size );		// next one guaranteed to be okay
  1073d7:	c7 43 04 00 00 00 00 	mov    DWORD PTR [ebx+0x4],0x0
			l_warningCount += 1;
  1073de:	83 15 44 ee 23 00 00 	adc    DWORD PTR ds:0x23ee44,0x0
	liballoc_unlock();		// release the lock
  1073e5:	e8 e6 04 00 00       	call   1078d0 <liballoc_unlock>
	return NULL;
  1073ea:	31 ed                	xor    ebp,ebp
  1073ec:	e9 9b fe ff ff       	jmp    10728c <malloc+0x2ec>
			maj->first->prev = (struct liballoc_minor*)((uintptr_t)maj + sizeof(struct liballoc_major) );
  1073f1:	8d 43 18             	lea    eax,[ebx+0x18]
			maj->usage 			+= size + sizeof( struct liballoc_minor );
  1073f4:	03 74 24 08          	add    esi,DWORD PTR [esp+0x8]
			ALIGN( p );
  1073f8:	8d 6b 40             	lea    ebp,[ebx+0x40]
			maj->first->size 	= size;
  1073fb:	8b 4c 24 10          	mov    ecx,DWORD PTR [esp+0x10]
			maj->first->req_size 	= req_size;
  1073ff:	8b 7c 24 50          	mov    edi,DWORD PTR [esp+0x50]
			maj->first->prev = (struct liballoc_minor*)((uintptr_t)maj + sizeof(struct liballoc_major) );
  107403:	89 02                	mov    DWORD PTR [edx],eax
			maj->first->prev->next = maj->first;
  107405:	89 53 1c             	mov    DWORD PTR [ebx+0x1c],edx
			l_inuse += size;
  107408:	31 d2                	xor    edx,edx
  10740a:	01 0d 48 ee 23 00    	add    DWORD PTR ds:0x23ee48,ecx
			maj->first = maj->first->prev;
  107410:	89 43 14             	mov    DWORD PTR [ebx+0x14],eax
			l_inuse += size;
  107413:	11 15 4c ee 23 00    	adc    DWORD PTR ds:0x23ee4c,edx
			ALIGN( p );
  107419:	89 ea                	mov    edx,ebp
			maj->first->magic 	= LIBALLOC_MAGIC;
  10741b:	c7 43 24 de c0 01 c0 	mov    DWORD PTR [ebx+0x24],0xc001c0de
			ALIGN( p );
  107422:	83 e2 0f             	and    edx,0xf
			maj->first->prev 	= NULL;
  107425:	c7 43 18 00 00 00 00 	mov    DWORD PTR [ebx+0x18],0x0
			maj->first->block 	= maj;
  10742c:	89 5b 20             	mov    DWORD PTR [ebx+0x20],ebx
			maj->first->size 	= size;
  10742f:	89 4b 28             	mov    DWORD PTR [ebx+0x28],ecx
			maj->first->req_size 	= req_size;
  107432:	89 7b 2c             	mov    DWORD PTR [ebx+0x2c],edi
			maj->usage 			+= size + sizeof( struct liballoc_minor );
  107435:	89 73 10             	mov    DWORD PTR [ebx+0x10],esi
			ALIGN( p );
  107438:	0f 85 3a fe ff ff    	jne    107278 <malloc+0x2d8>
  10743e:	e9 a9 fe ff ff       	jmp    1072ec <malloc+0x34c>
			l_warningCount += 1;
  107443:	83 05 40 ee 23 00 01 	add    DWORD PTR ds:0x23ee40,0x1
		l_memRoot = allocate_new_page( size );
  10744a:	c7 05 5c ee 23 00 00 	mov    DWORD PTR ds:0x23ee5c,0x0
  107451:	00 00 00 
			l_warningCount += 1;
  107454:	83 15 44 ee 23 00 00 	adc    DWORD PTR ds:0x23ee44,0x0
		  return NULL;
  10745b:	31 ed                	xor    ebp,ebp
		  liballoc_unlock();
  10745d:	e8 6e 04 00 00       	call   1078d0 <liballoc_unlock>
		  return NULL;
  107462:	e9 25 fe ff ff       	jmp    10728c <malloc+0x2ec>
  107467:	89 d8                	mov    eax,ebx
  107469:	e9 b8 fd ff ff       	jmp    107226 <malloc+0x286>
  10746e:	66 90                	xchg   ax,ax

00107470 <free>:




void PREFIX(free)(void *ptr)
{
  107470:	57                   	push   edi
  107471:	56                   	push   esi
  107472:	53                   	push   ebx
  107473:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
	struct liballoc_minor *min;
	struct liballoc_major *maj;

	if ( ptr == NULL ) 
  107477:	85 c0                	test   eax,eax
  107479:	0f 84 d1 00 00 00    	je     107550 <free+0xe0>
		FLUSH();
		#endif
		return;
	}

	UNALIGN( ptr );
  10747f:	0f be 50 f0          	movsx  edx,BYTE PTR [eax-0x10]
  107483:	89 c3                	mov    ebx,eax
  107485:	29 d3                	sub    ebx,edx
  107487:	83 fa 20             	cmp    edx,0x20
  10748a:	0f 43 d8             	cmovae ebx,eax

	liballoc_lock();		// lockit
  10748d:	e8 2e 04 00 00       	call   1078c0 <liballoc_lock>


	min = (struct liballoc_minor*)((uintptr_t)ptr - sizeof( struct liballoc_minor ));
  107492:	8d 43 e8             	lea    eax,[ebx-0x18]

	
	if ( min->magic != LIBALLOC_MAGIC ) 
  107495:	8b 50 0c             	mov    edx,DWORD PTR [eax+0xc]
  107498:	81 fa de c0 01 c0    	cmp    edx,0xc001c0de
  10749e:	74 30                	je     1074d0 <free+0x60>
	{
		l_errorCount += 1;
  1074a0:	83 05 38 ee 23 00 01 	add    DWORD PTR ds:0x23ee38,0x1

		// Check for overrun errors. For all bytes of LIBALLOC_MAGIC 
		if ( 
			((min->magic & 0xFFFFFF) == (LIBALLOC_MAGIC & 0xFFFFFF)) || 
  1074a7:	89 d0                	mov    eax,edx
		l_errorCount += 1;
  1074a9:	83 15 3c ee 23 00 00 	adc    DWORD PTR ds:0x23ee3c,0x0
			((min->magic & 0xFFFFFF) == (LIBALLOC_MAGIC & 0xFFFFFF)) || 
  1074b0:	25 ff ff ff 00       	and    eax,0xffffff
		if ( 
  1074b5:	3d de c0 01 00       	cmp    eax,0x1c0de
  1074ba:	74 7c                	je     107538 <free+0xc8>
			((min->magic & 0xFFFFFF) == (LIBALLOC_MAGIC & 0xFFFFFF)) || 
  1074bc:	66 81 fa de c0       	cmp    dx,0xc0de
  1074c1:	74 75                	je     107538 <free+0xc8>
			((min->magic & 0xFFFF) == (LIBALLOC_MAGIC & 0xFFFF)) || 
  1074c3:	80 fa de             	cmp    dl,0xde
  1074c6:	74 70                	je     107538 <free+0xc8>
	debug( "OK\n");
	FLUSH();
	#endif
	
	liballoc_unlock();		// release the lock
}
  1074c8:	5b                   	pop    ebx
  1074c9:	5e                   	pop    esi
  1074ca:	5f                   	pop    edi
		liballoc_unlock();		// release the lock
  1074cb:	e9 00 04 00 00       	jmp    1078d0 <liballoc_unlock>
		maj = min->block;
  1074d0:	8b 50 08             	mov    edx,DWORD PTR [eax+0x8]
		l_inuse -= min->size;
  1074d3:	8b 48 10             	mov    ecx,DWORD PTR [eax+0x10]
  1074d6:	31 ff                	xor    edi,edi
  1074d8:	29 0d 48 ee 23 00    	sub    DWORD PTR ds:0x23ee48,ecx
		if ( min->next != NULL ) min->next->prev = min->prev;
  1074de:	8b 70 04             	mov    esi,DWORD PTR [eax+0x4]
		l_inuse -= min->size;
  1074e1:	19 3d 4c ee 23 00    	sbb    DWORD PTR ds:0x23ee4c,edi
  1074e7:	8b 5b e8             	mov    ebx,DWORD PTR [ebx-0x18]
		maj->usage -= (min->size + sizeof( struct liballoc_minor ));
  1074ea:	8b 7a 10             	mov    edi,DWORD PTR [edx+0x10]
  1074ed:	29 cf                	sub    edi,ecx
  1074ef:	89 f9                	mov    ecx,edi
  1074f1:	83 e9 18             	sub    ecx,0x18
		if ( min->next != NULL ) min->next->prev = min->prev;
  1074f4:	85 f6                	test   esi,esi
		maj->usage -= (min->size + sizeof( struct liballoc_minor ));
  1074f6:	89 4a 10             	mov    DWORD PTR [edx+0x10],ecx
		min->magic  = LIBALLOC_DEAD;		// No mojo.
  1074f9:	c7 40 0c ad de ad de 	mov    DWORD PTR [eax+0xc],0xdeaddead
		if ( min->next != NULL ) min->next->prev = min->prev;
  107500:	74 04                	je     107506 <free+0x96>
  107502:	89 1e                	mov    DWORD PTR [esi],ebx
  107504:	8b 18                	mov    ebx,DWORD PTR [eax]
		if ( min->prev != NULL ) min->prev->next = min->next;
  107506:	85 db                	test   ebx,ebx
  107508:	74 5e                	je     107568 <free+0xf8>
  10750a:	89 73 04             	mov    DWORD PTR [ebx+0x4],esi
  10750d:	8b 72 14             	mov    esi,DWORD PTR [edx+0x14]
	if ( maj->first == NULL )	// Block completely unused.
  107510:	85 f6                	test   esi,esi
  107512:	a1 58 ee 23 00       	mov    eax,ds:0x23ee58
  107517:	74 57                	je     107570 <free+0x100>
		if ( l_bestBet != NULL )
  107519:	85 c0                	test   eax,eax
  10751b:	74 ab                	je     1074c8 <free+0x58>
			int bestSize = l_bestBet->size  - l_bestBet->usage;
  10751d:	8b 58 0c             	mov    ebx,DWORD PTR [eax+0xc]
  107520:	2b 58 10             	sub    ebx,DWORD PTR [eax+0x10]
			int majSize = maj->size - maj->usage;
  107523:	8b 42 0c             	mov    eax,DWORD PTR [edx+0xc]
  107526:	29 c8                	sub    eax,ecx
			if ( majSize > bestSize ) l_bestBet = maj;
  107528:	39 c3                	cmp    ebx,eax
  10752a:	7d 9c                	jge    1074c8 <free+0x58>
  10752c:	89 15 58 ee 23 00    	mov    DWORD PTR ds:0x23ee58,edx
  107532:	eb 94                	jmp    1074c8 <free+0x58>
  107534:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
			l_possibleOverruns += 1;
  107538:	83 05 30 ee 23 00 01 	add    DWORD PTR ds:0x23ee30,0x1
}
  10753f:	5b                   	pop    ebx
			l_possibleOverruns += 1;
  107540:	83 15 34 ee 23 00 00 	adc    DWORD PTR ds:0x23ee34,0x0
}
  107547:	5e                   	pop    esi
  107548:	5f                   	pop    edi
		liballoc_unlock();		// release the lock
  107549:	e9 82 03 00 00       	jmp    1078d0 <liballoc_unlock>
  10754e:	66 90                	xchg   ax,ax
		l_warningCount += 1;
  107550:	83 05 40 ee 23 00 01 	add    DWORD PTR ds:0x23ee40,0x1
}
  107557:	5b                   	pop    ebx
		l_warningCount += 1;
  107558:	83 15 44 ee 23 00 00 	adc    DWORD PTR ds:0x23ee44,0x0
}
  10755f:	5e                   	pop    esi
  107560:	5f                   	pop    edi
  107561:	c3                   	ret    
  107562:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
		if ( min->prev == NULL ) maj->first = min->next;	
  107568:	89 72 14             	mov    DWORD PTR [edx+0x14],esi
  10756b:	eb a3                	jmp    107510 <free+0xa0>
  10756d:	8d 76 00             	lea    esi,[esi+0x0]
		if ( l_memRoot == maj ) l_memRoot = maj->next;
  107570:	39 15 5c ee 23 00    	cmp    DWORD PTR ds:0x23ee5c,edx
  107576:	8b 4a 04             	mov    ecx,DWORD PTR [edx+0x4]
  107579:	74 45                	je     1075c0 <free+0x150>
		if ( l_bestBet == maj ) l_bestBet = NULL;
  10757b:	39 c2                	cmp    edx,eax
  10757d:	74 51                	je     1075d0 <free+0x160>
		if ( maj->prev != NULL ) maj->prev->next = maj->next;
  10757f:	8b 1a                	mov    ebx,DWORD PTR [edx]
  107581:	89 c8                	mov    eax,ecx
  107583:	85 db                	test   ebx,ebx
  107585:	74 06                	je     10758d <free+0x11d>
  107587:	89 4b 04             	mov    DWORD PTR [ebx+0x4],ecx
  10758a:	8b 42 04             	mov    eax,DWORD PTR [edx+0x4]
		if ( maj->next != NULL ) maj->next->prev = maj->prev;
  10758d:	85 c0                	test   eax,eax
  10758f:	74 02                	je     107593 <free+0x123>
  107591:	89 18                	mov    DWORD PTR [eax],ebx
		l_allocated -= maj->size;
  107593:	8b 4a 0c             	mov    ecx,DWORD PTR [edx+0xc]
  107596:	31 db                	xor    ebx,ebx
  107598:	29 0d 50 ee 23 00    	sub    DWORD PTR ds:0x23ee50,ecx
  10759e:	19 1d 54 ee 23 00    	sbb    DWORD PTR ds:0x23ee54,ebx
		liballoc_free( maj, maj->pages );
  1075a4:	83 ec 08             	sub    esp,0x8
  1075a7:	ff 72 08             	push   DWORD PTR [edx+0x8]
  1075aa:	52                   	push   edx
  1075ab:	e8 60 03 00 00       	call   107910 <liballoc_free>
  1075b0:	83 c4 10             	add    esp,0x10
  1075b3:	e9 10 ff ff ff       	jmp    1074c8 <free+0x58>
  1075b8:	90                   	nop
  1075b9:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
		if ( l_memRoot == maj ) l_memRoot = maj->next;
  1075c0:	89 0d 5c ee 23 00    	mov    DWORD PTR ds:0x23ee5c,ecx
  1075c6:	eb b3                	jmp    10757b <free+0x10b>
  1075c8:	90                   	nop
  1075c9:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
		if ( l_bestBet == maj ) l_bestBet = NULL;
  1075d0:	c7 05 58 ee 23 00 00 	mov    DWORD PTR ds:0x23ee58,0x0
  1075d7:	00 00 00 
  1075da:	eb a3                	jmp    10757f <free+0x10f>
  1075dc:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]

001075e0 <calloc>:




void* PREFIX(calloc)(size_t nobj, size_t size)
{
  1075e0:	55                   	push   ebp
  1075e1:	57                   	push   edi
  1075e2:	56                   	push   esi
  1075e3:	53                   	push   ebx
  1075e4:	83 ec 18             	sub    esp,0x18
       int real_size;
       void *p;

       real_size = nobj * size;
  1075e7:	8b 5c 24 30          	mov    ebx,DWORD PTR [esp+0x30]
  1075eb:	0f af 5c 24 2c       	imul   ebx,DWORD PTR [esp+0x2c]
       
       p = PREFIX(malloc)( real_size );
  1075f0:	53                   	push   ebx
  1075f1:	e8 aa f9 ff ff       	call   106fa0 <malloc>
	for ( i = 0; i < n ; i++)
  1075f6:	83 c4 10             	add    esp,0x10
  1075f9:	85 db                	test   ebx,ebx
  1075fb:	0f 84 b8 00 00 00    	je     1076b9 <calloc+0xd9>
  107601:	89 c1                	mov    ecx,eax
  107603:	8d 73 ff             	lea    esi,[ebx-0x1]
  107606:	bf 05 00 00 00       	mov    edi,0x5
  10760b:	f7 d9                	neg    ecx
  10760d:	83 e1 03             	and    ecx,0x3
  107610:	8d 51 03             	lea    edx,[ecx+0x3]
  107613:	83 fa 05             	cmp    edx,0x5
  107616:	0f 42 d7             	cmovb  edx,edi
  107619:	39 d6                	cmp    esi,edx
  10761b:	be 00 00 00 00       	mov    esi,0x0
  107620:	72 57                	jb     107679 <calloc+0x99>
  107622:	85 c9                	test   ecx,ecx
  107624:	74 24                	je     10764a <calloc+0x6a>
  107626:	83 f9 01             	cmp    ecx,0x1
		((char*)s)[i] = c;
  107629:	c6 00 00             	mov    BYTE PTR [eax],0x0
	for ( i = 0; i < n ; i++)
  10762c:	be 01 00 00 00       	mov    esi,0x1
  107631:	74 17                	je     10764a <calloc+0x6a>
  107633:	83 f9 03             	cmp    ecx,0x3
		((char*)s)[i] = c;
  107636:	c6 40 01 00          	mov    BYTE PTR [eax+0x1],0x0
	for ( i = 0; i < n ; i++)
  10763a:	be 02 00 00 00       	mov    esi,0x2
  10763f:	75 09                	jne    10764a <calloc+0x6a>
		((char*)s)[i] = c;
  107641:	c6 40 02 00          	mov    BYTE PTR [eax+0x2],0x0
	for ( i = 0; i < n ; i++)
  107645:	be 03 00 00 00       	mov    esi,0x3
  10764a:	89 dd                	mov    ebp,ebx
  10764c:	31 d2                	xor    edx,edx
  10764e:	29 cd                	sub    ebp,ecx
  107650:	01 c1                	add    ecx,eax
  107652:	89 ef                	mov    edi,ebp
  107654:	c1 ef 02             	shr    edi,0x2
  107657:	89 f6                	mov    esi,esi
  107659:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
		((char*)s)[i] = c;
  107660:	c7 04 91 00 00 00 00 	mov    DWORD PTR [ecx+edx*4],0x0
  107667:	83 c2 01             	add    edx,0x1
  10766a:	39 d7                	cmp    edi,edx
  10766c:	77 f2                	ja     107660 <calloc+0x80>
  10766e:	89 ea                	mov    edx,ebp
  107670:	83 e2 fc             	and    edx,0xfffffffc
  107673:	01 d6                	add    esi,edx
  107675:	39 d5                	cmp    ebp,edx
  107677:	74 40                	je     1076b9 <calloc+0xd9>
	for ( i = 0; i < n ; i++)
  107679:	8d 56 01             	lea    edx,[esi+0x1]
		((char*)s)[i] = c;
  10767c:	c6 04 30 00          	mov    BYTE PTR [eax+esi*1],0x0
	for ( i = 0; i < n ; i++)
  107680:	39 d3                	cmp    ebx,edx
  107682:	76 35                	jbe    1076b9 <calloc+0xd9>
  107684:	8d 56 02             	lea    edx,[esi+0x2]
		((char*)s)[i] = c;
  107687:	c6 44 30 01 00       	mov    BYTE PTR [eax+esi*1+0x1],0x0
	for ( i = 0; i < n ; i++)
  10768c:	39 d3                	cmp    ebx,edx
  10768e:	76 29                	jbe    1076b9 <calloc+0xd9>
  107690:	8d 56 03             	lea    edx,[esi+0x3]
		((char*)s)[i] = c;
  107693:	c6 44 30 02 00       	mov    BYTE PTR [eax+esi*1+0x2],0x0
	for ( i = 0; i < n ; i++)
  107698:	39 d3                	cmp    ebx,edx
  10769a:	76 1d                	jbe    1076b9 <calloc+0xd9>
  10769c:	8d 56 04             	lea    edx,[esi+0x4]
		((char*)s)[i] = c;
  10769f:	c6 44 30 03 00       	mov    BYTE PTR [eax+esi*1+0x3],0x0
	for ( i = 0; i < n ; i++)
  1076a4:	39 d3                	cmp    ebx,edx
  1076a6:	76 11                	jbe    1076b9 <calloc+0xd9>
  1076a8:	8d 56 05             	lea    edx,[esi+0x5]
		((char*)s)[i] = c;
  1076ab:	c6 44 30 04 00       	mov    BYTE PTR [eax+esi*1+0x4],0x0
	for ( i = 0; i < n ; i++)
  1076b0:	39 d3                	cmp    ebx,edx
  1076b2:	76 05                	jbe    1076b9 <calloc+0xd9>
		((char*)s)[i] = c;
  1076b4:	c6 44 30 05 00       	mov    BYTE PTR [eax+esi*1+0x5],0x0

       liballoc_memset( p, 0, real_size );

       return p;
}
  1076b9:	83 c4 0c             	add    esp,0xc
  1076bc:	5b                   	pop    ebx
  1076bd:	5e                   	pop    esi
  1076be:	5f                   	pop    edi
  1076bf:	5d                   	pop    ebp
  1076c0:	c3                   	ret    
  1076c1:	eb 0d                	jmp    1076d0 <realloc>
  1076c3:	90                   	nop
  1076c4:	90                   	nop
  1076c5:	90                   	nop
  1076c6:	90                   	nop
  1076c7:	90                   	nop
  1076c8:	90                   	nop
  1076c9:	90                   	nop
  1076ca:	90                   	nop
  1076cb:	90                   	nop
  1076cc:	90                   	nop
  1076cd:	90                   	nop
  1076ce:	90                   	nop
  1076cf:	90                   	nop

001076d0 <realloc>:



void*   PREFIX(realloc)(void *p, size_t size)
{
  1076d0:	57                   	push   edi
  1076d1:	56                   	push   esi
  1076d2:	53                   	push   ebx
  1076d3:	83 ec 10             	sub    esp,0x10
  1076d6:	8b 7c 24 24          	mov    edi,DWORD PTR [esp+0x24]
  1076da:	8b 5c 24 20          	mov    ebx,DWORD PTR [esp+0x20]
	void *ptr;
	struct liballoc_minor *min;
	unsigned int real_size;
	
	// Honour the case of size == 0 => free old and return NULL
	if ( size == 0 )
  1076de:	85 ff                	test   edi,edi
  1076e0:	0f 84 4a 01 00 00    	je     107830 <realloc+0x160>
		PREFIX(free)( p );
		return NULL;
	}

	// In the case of a NULL pointer, return a simple malloc.
	if ( p == NULL ) return PREFIX(malloc)( size );
  1076e6:	85 db                	test   ebx,ebx
  1076e8:	0f 84 d2 00 00 00    	je     1077c0 <realloc+0xf0>

	// Unalign the pointer if required.
	ptr = p;
	UNALIGN(ptr);
  1076ee:	0f be 53 f0          	movsx  edx,BYTE PTR [ebx-0x10]
  1076f2:	89 d8                	mov    eax,ebx
  1076f4:	29 d0                	sub    eax,edx
  1076f6:	83 fa 20             	cmp    edx,0x20
  1076f9:	0f 43 c3             	cmovae eax,ebx
  1076fc:	89 c6                	mov    esi,eax

	liballoc_lock();		// lockit
  1076fe:	e8 bd 01 00 00       	call   1078c0 <liballoc_lock>

		min = (struct liballoc_minor*)((uintptr_t)ptr - sizeof( struct liballoc_minor ));
  107703:	8d 46 e8             	lea    eax,[esi-0x18]

		// Ensure it is a valid structure.
		if ( min->magic != LIBALLOC_MAGIC ) 
  107706:	8b 50 0c             	mov    edx,DWORD PTR [eax+0xc]
  107709:	81 fa de c0 01 c0    	cmp    edx,0xc001c0de
  10770f:	0f 85 bb 00 00 00    	jne    1077d0 <realloc+0x100>
			return NULL;
		}	
		
		// Definitely a memory block.
		
		real_size = min->req_size;
  107715:	8b 70 14             	mov    esi,DWORD PTR [eax+0x14]

		if ( real_size >= size ) 
  107718:	39 f7                	cmp    edi,esi
  10771a:	0f 86 88 00 00 00    	jbe    1077a8 <realloc+0xd8>
			min->req_size = size;
			liballoc_unlock();
			return p;
		}

	liballoc_unlock();
  107720:	e8 ab 01 00 00       	call   1078d0 <liballoc_unlock>

	// If we got here then we're reallocating to a block bigger than us.
	ptr = PREFIX(malloc)( size );					// We need to allocate new memory
  107725:	83 ec 0c             	sub    esp,0xc
  107728:	57                   	push   edi
  107729:	e8 72 f8 ff ff       	call   106fa0 <malloc>
  while ( n >= sizeof(unsigned int) )
  10772e:	83 c4 10             	add    esp,0x10
  107731:	83 fe 03             	cmp    esi,0x3
  107734:	0f 86 e6 00 00 00    	jbe    107820 <realloc+0x150>
  10773a:	8d 56 fc             	lea    edx,[esi-0x4]
  10773d:	c1 ea 02             	shr    edx,0x2
  107740:	8d 3c 95 04 00 00 00 	lea    edi,[edx*4+0x4]
  107747:	31 d2                	xor    edx,edx
  107749:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
      *ldest++ = *lsrc++;
  107750:	8b 0c 13             	mov    ecx,DWORD PTR [ebx+edx*1]
  107753:	89 0c 10             	mov    DWORD PTR [eax+edx*1],ecx
  107756:	83 c2 04             	add    edx,0x4
  while ( n >= sizeof(unsigned int) )
  107759:	39 fa                	cmp    edx,edi
  10775b:	75 f3                	jne    107750 <realloc+0x80>
  10775d:	8d 3c 13             	lea    edi,[ebx+edx*1]
  107760:	83 e6 03             	and    esi,0x3
  107763:	01 c2                	add    edx,eax
  while ( n > 0 )
  107765:	85 f6                	test   esi,esi
  107767:	74 1d                	je     107786 <realloc+0xb6>
      *cdest++ = *csrc++;
  107769:	0f b6 0f             	movzx  ecx,BYTE PTR [edi]
  while ( n > 0 )
  10776c:	83 fe 01             	cmp    esi,0x1
      *cdest++ = *csrc++;
  10776f:	88 0a                	mov    BYTE PTR [edx],cl
  while ( n > 0 )
  107771:	74 13                	je     107786 <realloc+0xb6>
      *cdest++ = *csrc++;
  107773:	0f b6 4f 01          	movzx  ecx,BYTE PTR [edi+0x1]
  while ( n > 0 )
  107777:	83 fe 02             	cmp    esi,0x2
      *cdest++ = *csrc++;
  10777a:	88 4a 01             	mov    BYTE PTR [edx+0x1],cl
  while ( n > 0 )
  10777d:	74 07                	je     107786 <realloc+0xb6>
      *cdest++ = *csrc++;
  10777f:	0f b6 4f 02          	movzx  ecx,BYTE PTR [edi+0x2]
  107783:	88 4a 02             	mov    BYTE PTR [edx+0x2],cl
  107786:	89 44 24 0c          	mov    DWORD PTR [esp+0xc],eax
	liballoc_memcpy( ptr, p, real_size );
	PREFIX(free)( p );
  10778a:	83 ec 0c             	sub    esp,0xc
  10778d:	53                   	push   ebx
  10778e:	e8 dd fc ff ff       	call   107470 <free>

	return ptr;
  107793:	83 c4 10             	add    esp,0x10
  107796:	8b 44 24 0c          	mov    eax,DWORD PTR [esp+0xc]
}
  10779a:	83 c4 10             	add    esp,0x10
  10779d:	5b                   	pop    ebx
  10779e:	5e                   	pop    esi
  10779f:	5f                   	pop    edi
  1077a0:	c3                   	ret    
  1077a1:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
			min->req_size = size;
  1077a8:	89 78 14             	mov    DWORD PTR [eax+0x14],edi
			liballoc_unlock();
  1077ab:	e8 20 01 00 00       	call   1078d0 <liballoc_unlock>
			return p;
  1077b0:	89 d8                	mov    eax,ebx
}
  1077b2:	83 c4 10             	add    esp,0x10
  1077b5:	5b                   	pop    ebx
  1077b6:	5e                   	pop    esi
  1077b7:	5f                   	pop    edi
  1077b8:	c3                   	ret    
  1077b9:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
	if ( p == NULL ) return PREFIX(malloc)( size );
  1077c0:	89 7c 24 20          	mov    DWORD PTR [esp+0x20],edi
}
  1077c4:	83 c4 10             	add    esp,0x10
  1077c7:	5b                   	pop    ebx
  1077c8:	5e                   	pop    esi
  1077c9:	5f                   	pop    edi
	if ( p == NULL ) return PREFIX(malloc)( size );
  1077ca:	e9 d1 f7 ff ff       	jmp    106fa0 <malloc>
  1077cf:	90                   	nop
			l_errorCount += 1;
  1077d0:	83 05 38 ee 23 00 01 	add    DWORD PTR ds:0x23ee38,0x1
				((min->magic & 0xFFFFFF) == (LIBALLOC_MAGIC & 0xFFFFFF)) || 
  1077d7:	89 d0                	mov    eax,edx
			l_errorCount += 1;
  1077d9:	83 15 3c ee 23 00 00 	adc    DWORD PTR ds:0x23ee3c,0x0
				((min->magic & 0xFFFFFF) == (LIBALLOC_MAGIC & 0xFFFFFF)) || 
  1077e0:	25 ff ff ff 00       	and    eax,0xffffff
			if ( 
  1077e5:	3d de c0 01 00       	cmp    eax,0x1c0de
  1077ea:	74 24                	je     107810 <realloc+0x140>
				((min->magic & 0xFFFFFF) == (LIBALLOC_MAGIC & 0xFFFFFF)) || 
  1077ec:	66 81 fa de c0       	cmp    dx,0xc0de
  1077f1:	74 1d                	je     107810 <realloc+0x140>
				((min->magic & 0xFFFF) == (LIBALLOC_MAGIC & 0xFFFF)) || 
  1077f3:	80 fa de             	cmp    dl,0xde
  1077f6:	74 18                	je     107810 <realloc+0x140>
			liballoc_unlock();		// release the lock
  1077f8:	e8 d3 00 00 00       	call   1078d0 <liballoc_unlock>
}
  1077fd:	83 c4 10             	add    esp,0x10
			return NULL;
  107800:	31 c0                	xor    eax,eax
}
  107802:	5b                   	pop    ebx
  107803:	5e                   	pop    esi
  107804:	5f                   	pop    edi
  107805:	c3                   	ret    
  107806:	8d 76 00             	lea    esi,[esi+0x0]
  107809:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
				l_possibleOverruns += 1;
  107810:	83 05 30 ee 23 00 01 	add    DWORD PTR ds:0x23ee30,0x1
  107817:	83 15 34 ee 23 00 00 	adc    DWORD PTR ds:0x23ee34,0x0
  10781e:	eb d8                	jmp    1077f8 <realloc+0x128>
	ptr = PREFIX(malloc)( size );					// We need to allocate new memory
  107820:	89 c2                	mov    edx,eax
  while ( n >= sizeof(unsigned int) )
  107822:	89 df                	mov    edi,ebx
  107824:	e9 3c ff ff ff       	jmp    107765 <realloc+0x95>
  107829:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
		PREFIX(free)( p );
  107830:	83 ec 0c             	sub    esp,0xc
  107833:	53                   	push   ebx
  107834:	e8 37 fc ff ff       	call   107470 <free>
		return NULL;
  107839:	83 c4 10             	add    esp,0x10
  10783c:	31 c0                	xor    eax,eax
  10783e:	e9 6f ff ff ff       	jmp    1077b2 <realloc+0xe2>
  107843:	66 90                	xchg   ax,ax
  107845:	66 90                	xchg   ax,ax
  107847:	66 90                	xchg   ax,ax
  107849:	66 90                	xchg   ax,ax
  10784b:	66 90                	xchg   ax,ax
  10784d:	66 90                	xchg   ax,ax
  10784f:	90                   	nop

00107850 <putchar>:
#include <string.h>

#include "kernel/serial.h"

int putchar(int chr)
{
  107850:	53                   	push   ebx
  107851:	83 ec 14             	sub    esp,0x14
  107854:	8b 5c 24 1c          	mov    ebx,DWORD PTR [esp+0x1c]
    serial_putc(chr);
  107858:	0f be c3             	movsx  eax,bl
  10785b:	50                   	push   eax
  10785c:	e8 4f bf ff ff       	call   1037b0 <serial_putc>
    return chr;
}
  107861:	83 c4 18             	add    esp,0x18
  107864:	89 d8                	mov    eax,ebx
  107866:	5b                   	pop    ebx
  107867:	c3                   	ret    
  107868:	90                   	nop
  107869:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]

00107870 <getchar>:

int getchar()
{
  107870:	83 ec 0c             	sub    esp,0xc
    return serial_getc();
  107873:	e8 08 bf ff ff       	call   103780 <serial_getc>
}
  107878:	83 c4 0c             	add    esp,0xc
    return serial_getc();
  10787b:	0f be c0             	movsx  eax,al
}
  10787e:	c3                   	ret    
  10787f:	90                   	nop

00107880 <gets>:

char * gets(char * str)
{
  107880:	53                   	push   ebx
  107881:	83 ec 10             	sub    esp,0x10
  107884:	8b 5c 24 18          	mov    ebx,DWORD PTR [esp+0x18]
   serial_read(str, 100);
  107888:	6a 64                	push   0x64
  10788a:	53                   	push   ebx
  10788b:	e8 60 bf ff ff       	call   1037f0 <serial_read>
   return str;
}
  107890:	83 c4 18             	add    esp,0x18
  107893:	89 d8                	mov    eax,ebx
  107895:	5b                   	pop    ebx
  107896:	c3                   	ret    
  107897:	89 f6                	mov    esi,esi
  107899:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

001078a0 <puts>:

int puts(const char * str)
{
  1078a0:	53                   	push   ebx
  1078a1:	83 ec 14             	sub    esp,0x14
  1078a4:	8b 5c 24 1c          	mov    ebx,DWORD PTR [esp+0x1c]
    serial_print((char *)str);
  1078a8:	53                   	push   ebx
  1078a9:	e8 62 bf ff ff       	call   103810 <serial_print>
    return strlen(str);
  1078ae:	89 5c 24 20          	mov    DWORD PTR [esp+0x20],ebx
  1078b2:	83 c4 18             	add    esp,0x18
  1078b5:	5b                   	pop    ebx
    return strlen(str);
  1078b6:	e9 05 e8 ff ff       	jmp    1060c0 <strlen>
  1078bb:	66 90                	xchg   ax,ax
  1078bd:	66 90                	xchg   ax,ax
  1078bf:	90                   	nop

001078c0 <liballoc_lock>:
#include "sync/atomic.h"
#include "kernel/memory.h"
#include "kernel/logger.h"

int liballoc_lock()
{
  1078c0:	83 ec 0c             	sub    esp,0xc
    atomic_begin();
  1078c3:	e8 78 05 00 00       	call   107e40 <atomic_begin>
    return 0;
}
  1078c8:	31 c0                	xor    eax,eax
  1078ca:	83 c4 0c             	add    esp,0xc
  1078cd:	c3                   	ret    
  1078ce:	66 90                	xchg   ax,ax

001078d0 <liballoc_unlock>:

int liballoc_unlock()
{
  1078d0:	83 ec 0c             	sub    esp,0xc
    atomic_end();
  1078d3:	e8 88 05 00 00       	call   107e60 <atomic_end>
    return 0;
}
  1078d8:	31 c0                	xor    eax,eax
  1078da:	83 c4 0c             	add    esp,0xc
  1078dd:	c3                   	ret    
  1078de:	66 90                	xchg   ax,ax

001078e0 <liballoc_alloc>:

void* liballoc_alloc(size_t size)
{
  1078e0:	56                   	push   esi
  1078e1:	53                   	push   ebx
  1078e2:	83 ec 10             	sub    esp,0x10
  1078e5:	8b 74 24 1c          	mov    esi,DWORD PTR [esp+0x1c]
    void* p = (void*)memory_alloc(size);
  1078e9:	56                   	push   esi
  1078ea:	e8 a1 b3 ff ff       	call   102c90 <memory_alloc>
    log("LIBALLOC alloc size: %d at 0x%x.", size, p);
  1078ef:	83 c4 0c             	add    esp,0xc
    void* p = (void*)memory_alloc(size);
  1078f2:	89 c3                	mov    ebx,eax
    log("LIBALLOC alloc size: %d at 0x%x.", size, p);
  1078f4:	50                   	push   eax
  1078f5:	56                   	push   esi
  1078f6:	68 10 69 11 00       	push   0x116910
  1078fb:	e8 a0 9d ff ff       	call   1016a0 <log>
    return p;
}
  107900:	83 c4 14             	add    esp,0x14
  107903:	89 d8                	mov    eax,ebx
  107905:	5b                   	pop    ebx
  107906:	5e                   	pop    esi
  107907:	c3                   	ret    
  107908:	90                   	nop
  107909:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]

00107910 <liballoc_free>:

int liballoc_free(void* p,size_t size)
{
  107910:	56                   	push   esi
  107911:	53                   	push   ebx
  107912:	83 ec 08             	sub    esp,0x8
  107915:	8b 5c 24 14          	mov    ebx,DWORD PTR [esp+0x14]
  107919:	8b 74 24 18          	mov    esi,DWORD PTR [esp+0x18]
    log("LIBALLOC free: %d at 0x%x.", size, p);
  10791d:	53                   	push   ebx
  10791e:	56                   	push   esi
  10791f:	68 94 24 11 00       	push   0x112494
  107924:	e8 77 9d ff ff       	call   1016a0 <log>
    memory_free((uint)p, size);
  107929:	58                   	pop    eax
  10792a:	5a                   	pop    edx
  10792b:	56                   	push   esi
  10792c:	53                   	push   ebx
  10792d:	e8 ae b4 ff ff       	call   102de0 <memory_free>
    return 0;
  107932:	83 c4 14             	add    esp,0x14
  107935:	31 c0                	xor    eax,eax
  107937:	5b                   	pop    ebx
  107938:	5e                   	pop    esi
  107939:	c3                   	ret    
  10793a:	66 90                	xchg   ax,ax
  10793c:	66 90                	xchg   ax,ax
  10793e:	66 90                	xchg   ax,ax

00107940 <get_file_size>:
                        /* 500 */
}
tar_rawblock_t;

uint get_file_size(tar_rawblock_t *header)
{
  107940:	8b 54 24 04          	mov    edx,DWORD PTR [esp+0x4]
    unsigned int size = 0;
    unsigned int count = 1;

    for (uint j = 11; j > 0; j--, count *= 8)
    {
        size += ((header->size[j - 1] - '0') * count);
  107944:	0f be 82 84 00 00 00 	movsx  eax,BYTE PTR [edx+0x84]
  10794b:	0f be 8a 85 00 00 00 	movsx  ecx,BYTE PTR [edx+0x85]
  107952:	83 e8 30             	sub    eax,0x30
  107955:	c1 e0 06             	shl    eax,0x6
  107958:	8d 8c c8 80 fe ff ff 	lea    ecx,[eax+ecx*8-0x180]
  10795f:	0f be 82 86 00 00 00 	movsx  eax,BYTE PTR [edx+0x86]
  107966:	8d 44 01 d0          	lea    eax,[ecx+eax*1-0x30]
  10796a:	0f be 8a 83 00 00 00 	movsx  ecx,BYTE PTR [edx+0x83]
  107971:	83 e9 30             	sub    ecx,0x30
  107974:	c1 e1 09             	shl    ecx,0x9
  107977:	01 c1                	add    ecx,eax
  107979:	0f be 82 82 00 00 00 	movsx  eax,BYTE PTR [edx+0x82]
  107980:	83 e8 30             	sub    eax,0x30
  107983:	c1 e0 0c             	shl    eax,0xc
  107986:	01 c1                	add    ecx,eax
  107988:	0f be 82 81 00 00 00 	movsx  eax,BYTE PTR [edx+0x81]
  10798f:	83 e8 30             	sub    eax,0x30
  107992:	c1 e0 0f             	shl    eax,0xf
  107995:	01 c8                	add    eax,ecx
  107997:	0f be 8a 80 00 00 00 	movsx  ecx,BYTE PTR [edx+0x80]
  10799e:	83 e9 30             	sub    ecx,0x30
  1079a1:	c1 e1 12             	shl    ecx,0x12
  1079a4:	01 c8                	add    eax,ecx
  1079a6:	0f be 4a 7f          	movsx  ecx,BYTE PTR [edx+0x7f]
  1079aa:	83 e9 30             	sub    ecx,0x30
  1079ad:	c1 e1 15             	shl    ecx,0x15
  1079b0:	01 c1                	add    ecx,eax
  1079b2:	0f be 42 7e          	movsx  eax,BYTE PTR [edx+0x7e]
  1079b6:	83 e8 30             	sub    eax,0x30
  1079b9:	c1 e0 18             	shl    eax,0x18
  1079bc:	01 c1                	add    ecx,eax
  1079be:	0f be 42 7d          	movsx  eax,BYTE PTR [edx+0x7d]
  1079c2:	0f be 52 7c          	movsx  edx,BYTE PTR [edx+0x7c]
  1079c6:	83 e8 30             	sub    eax,0x30
  1079c9:	c1 e2 1e             	shl    edx,0x1e
  1079cc:	c1 e0 1b             	shl    eax,0x1b
  1079cf:	01 c8                	add    eax,ecx
  1079d1:	01 d0                	add    eax,edx
    }

    return size;
}
  1079d3:	c3                   	ret    
  1079d4:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
  1079da:	8d bf 00 00 00 00    	lea    edi,[edi+0x0]

001079e0 <tar_read>:

bool tar_read(void *tarfile, tar_block_t *block, uint index)
{
  1079e0:	56                   	push   esi
  1079e1:	53                   	push   ebx
  1079e2:	83 ec 04             	sub    esp,0x4
  1079e5:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
  1079e9:	8b 5c 24 10          	mov    ebx,DWORD PTR [esp+0x10]
    tar_rawblock_t *header = (tar_rawblock_t *)tarfile;

    for (size_t i = 0; i < index; i++)
  1079ed:	85 c9                	test   ecx,ecx
  1079ef:	0f b6 03             	movzx  eax,BYTE PTR [ebx]
  1079f2:	0f 84 d2 00 00 00    	je     107aca <tar_read+0xea>
    {
        if (header->name[0] == '\0')
  1079f8:	84 c0                	test   al,al
  1079fa:	0f 84 e0 00 00 00    	je     107ae0 <tar_read+0x100>
    for (size_t i = 0; i < index; i++)
  107a00:	31 d2                	xor    edx,edx
  107a02:	eb 0c                	jmp    107a10 <tar_read+0x30>
  107a04:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
        if (header->name[0] == '\0')
  107a08:	84 c0                	test   al,al
  107a0a:	0f 84 d0 00 00 00    	je     107ae0 <tar_read+0x100>
        size += ((header->size[j - 1] - '0') * count);
  107a10:	0f be 83 84 00 00 00 	movsx  eax,BYTE PTR [ebx+0x84]
  107a17:	0f be b3 85 00 00 00 	movsx  esi,BYTE PTR [ebx+0x85]
  107a1e:	83 e8 30             	sub    eax,0x30
  107a21:	c1 e0 06             	shl    eax,0x6
  107a24:	8d b4 f0 80 fe ff ff 	lea    esi,[eax+esi*8-0x180]
  107a2b:	0f be 83 86 00 00 00 	movsx  eax,BYTE PTR [ebx+0x86]
  107a32:	8d 44 06 d0          	lea    eax,[esi+eax*1-0x30]
  107a36:	0f be b3 83 00 00 00 	movsx  esi,BYTE PTR [ebx+0x83]
  107a3d:	83 ee 30             	sub    esi,0x30
  107a40:	c1 e6 09             	shl    esi,0x9
  107a43:	01 c6                	add    esi,eax
  107a45:	0f be 83 82 00 00 00 	movsx  eax,BYTE PTR [ebx+0x82]
  107a4c:	83 e8 30             	sub    eax,0x30
  107a4f:	c1 e0 0c             	shl    eax,0xc
  107a52:	01 c6                	add    esi,eax
  107a54:	0f be 83 81 00 00 00 	movsx  eax,BYTE PTR [ebx+0x81]
  107a5b:	83 e8 30             	sub    eax,0x30
  107a5e:	c1 e0 0f             	shl    eax,0xf
  107a61:	01 f0                	add    eax,esi
  107a63:	0f be b3 80 00 00 00 	movsx  esi,BYTE PTR [ebx+0x80]
  107a6a:	83 ee 30             	sub    esi,0x30
  107a6d:	c1 e6 12             	shl    esi,0x12
  107a70:	01 f0                	add    eax,esi
  107a72:	0f be 73 7f          	movsx  esi,BYTE PTR [ebx+0x7f]
  107a76:	83 ee 30             	sub    esi,0x30
  107a79:	c1 e6 15             	shl    esi,0x15
  107a7c:	01 c6                	add    esi,eax
  107a7e:	0f be 43 7e          	movsx  eax,BYTE PTR [ebx+0x7e]
  107a82:	83 e8 30             	sub    eax,0x30
  107a85:	c1 e0 18             	shl    eax,0x18
  107a88:	01 c6                	add    esi,eax
  107a8a:	0f be 43 7d          	movsx  eax,BYTE PTR [ebx+0x7d]
  107a8e:	83 e8 30             	sub    eax,0x30
  107a91:	c1 e0 1b             	shl    eax,0x1b
  107a94:	01 f0                	add    eax,esi
  107a96:	0f be 73 7c          	movsx  esi,BYTE PTR [ebx+0x7c]
  107a9a:	c1 e6 1e             	shl    esi,0x1e
  107a9d:	01 f0                	add    eax,esi
            return false;

        u32 size = get_file_size(header);

        header = (tar_rawblock_t *)((char *)header + ((size / 512) + 1) * 512);
  107a9f:	89 c6                	mov    esi,eax
  107aa1:	81 e6 00 fe ff ff    	and    esi,0xfffffe00

        if (size % 512)
            header = (tar_rawblock_t *)((char *)header + 512);
  107aa7:	a9 ff 01 00 00       	test   eax,0x1ff
        header = (tar_rawblock_t *)((char *)header + ((size / 512) + 1) * 512);
  107aac:	8d 9c 33 00 02 00 00 	lea    ebx,[ebx+esi*1+0x200]
            header = (tar_rawblock_t *)((char *)header + 512);
  107ab3:	8d b3 00 02 00 00    	lea    esi,[ebx+0x200]
  107ab9:	0f 45 de             	cmovne ebx,esi
    for (size_t i = 0; i < index; i++)
  107abc:	83 c2 01             	add    edx,0x1
  107abf:	39 d1                	cmp    ecx,edx
  107ac1:	0f b6 03             	movzx  eax,BYTE PTR [ebx]
  107ac4:	0f 85 3e ff ff ff    	jne    107a08 <tar_read+0x28>
            return false;
  107aca:	31 d2                	xor    edx,edx
    }

    if (header->name[0] == '\0')
  107acc:	84 c0                	test   al,al
  107ace:	75 20                	jne    107af0 <tar_read+0x110>
    memcpy(block->name, header->name, 100);
    block->size = get_file_size(header);
    block->data = (char *)header + 512;

    return true;
}
  107ad0:	83 c4 04             	add    esp,0x4
  107ad3:	89 d0                	mov    eax,edx
  107ad5:	5b                   	pop    ebx
  107ad6:	5e                   	pop    esi
  107ad7:	c3                   	ret    
  107ad8:	90                   	nop
  107ad9:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
  107ae0:	83 c4 04             	add    esp,0x4
            return false;
  107ae3:	31 d2                	xor    edx,edx
}
  107ae5:	89 d0                	mov    eax,edx
  107ae7:	5b                   	pop    ebx
  107ae8:	5e                   	pop    esi
  107ae9:	c3                   	ret    
  107aea:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
    memcpy(block->name, header->name, 100);
  107af0:	83 ec 04             	sub    esp,0x4
  107af3:	6a 64                	push   0x64
  107af5:	53                   	push   ebx
    block->data = (char *)header + 512;
  107af6:	81 c3 00 02 00 00    	add    ebx,0x200
    memcpy(block->name, header->name, 100);
  107afc:	ff 74 24 20          	push   DWORD PTR [esp+0x20]
  107b00:	e8 0b db ff ff       	call   105610 <memcpy>
        size += ((header->size[j - 1] - '0') * count);
  107b05:	0f be 83 84 fe ff ff 	movsx  eax,BYTE PTR [ebx-0x17c]
  107b0c:	0f be 93 85 fe ff ff 	movsx  edx,BYTE PTR [ebx-0x17b]
  107b13:	8b 4c 24 24          	mov    ecx,DWORD PTR [esp+0x24]
    return true;
  107b17:	83 c4 10             	add    esp,0x10
        size += ((header->size[j - 1] - '0') * count);
  107b1a:	83 e8 30             	sub    eax,0x30
  107b1d:	c1 e0 06             	shl    eax,0x6
  107b20:	8d 84 d0 80 fe ff ff 	lea    eax,[eax+edx*8-0x180]
  107b27:	0f be 93 83 fe ff ff 	movsx  edx,BYTE PTR [ebx-0x17d]
  107b2e:	83 ea 30             	sub    edx,0x30
  107b31:	c1 e2 09             	shl    edx,0x9
  107b34:	01 d0                	add    eax,edx
  107b36:	0f be 93 82 fe ff ff 	movsx  edx,BYTE PTR [ebx-0x17e]
  107b3d:	83 ea 30             	sub    edx,0x30
  107b40:	c1 e2 0c             	shl    edx,0xc
  107b43:	01 d0                	add    eax,edx
  107b45:	0f be 93 81 fe ff ff 	movsx  edx,BYTE PTR [ebx-0x17f]
  107b4c:	83 ea 30             	sub    edx,0x30
  107b4f:	c1 e2 0f             	shl    edx,0xf
  107b52:	01 d0                	add    eax,edx
  107b54:	0f be 93 80 fe ff ff 	movsx  edx,BYTE PTR [ebx-0x180]
  107b5b:	83 ea 30             	sub    edx,0x30
  107b5e:	c1 e2 12             	shl    edx,0x12
  107b61:	01 d0                	add    eax,edx
  107b63:	0f be 93 7f fe ff ff 	movsx  edx,BYTE PTR [ebx-0x181]
  107b6a:	83 ea 30             	sub    edx,0x30
  107b6d:	c1 e2 15             	shl    edx,0x15
  107b70:	01 d0                	add    eax,edx
  107b72:	0f be 93 7e fe ff ff 	movsx  edx,BYTE PTR [ebx-0x182]
  107b79:	83 ea 30             	sub    edx,0x30
  107b7c:	c1 e2 18             	shl    edx,0x18
  107b7f:	01 d0                	add    eax,edx
  107b81:	0f be 93 7d fe ff ff 	movsx  edx,BYTE PTR [ebx-0x183]
  107b88:	83 ea 30             	sub    edx,0x30
  107b8b:	c1 e2 1b             	shl    edx,0x1b
  107b8e:	01 d0                	add    eax,edx
  107b90:	0f be 93 7c fe ff ff 	movsx  edx,BYTE PTR [ebx-0x184]
  107b97:	c1 e2 1e             	shl    edx,0x1e
  107b9a:	01 d0                	add    eax,edx
  107b9c:	0f be 93 86 fe ff ff 	movsx  edx,BYTE PTR [ebx-0x17a]
    block->data = (char *)header + 512;
  107ba3:	89 59 68             	mov    DWORD PTR [ecx+0x68],ebx
        size += ((header->size[j - 1] - '0') * count);
  107ba6:	8d 44 10 d0          	lea    eax,[eax+edx*1-0x30]
    return true;
  107baa:	ba 01 00 00 00       	mov    edx,0x1
        size += ((header->size[j - 1] - '0') * count);
  107baf:	89 41 64             	mov    DWORD PTR [ecx+0x64],eax
}
  107bb2:	83 c4 04             	add    esp,0x4
  107bb5:	89 d0                	mov    eax,edx
  107bb7:	5b                   	pop    ebx
  107bb8:	5e                   	pop    esi
  107bb9:	c3                   	ret    
  107bba:	66 90                	xchg   ax,ax
  107bbc:	66 90                	xchg   ax,ax
  107bbe:	66 90                	xchg   ax,ax

00107bc0 <createnode>:
// Single Linked sllist_t
#include "ds/sllist.h"

sllist_node_t *createnode(int data)
{
  107bc0:	83 ec 18             	sub    esp,0x18
    sllist_node_t *newsllist_node_t = malloc(sizeof(sllist_node_t));
  107bc3:	6a 08                	push   0x8
  107bc5:	e8 d6 f3 ff ff       	call   106fa0 <malloc>

    newsllist_node_t->data = data;
  107bca:	8b 54 24 20          	mov    edx,DWORD PTR [esp+0x20]
    newsllist_node_t->next = NULL;
  107bce:	c7 40 04 00 00 00 00 	mov    DWORD PTR [eax+0x4],0x0
    newsllist_node_t->data = data;
  107bd5:	89 10                	mov    DWORD PTR [eax],edx

    return newsllist_node_t;
}
  107bd7:	83 c4 1c             	add    esp,0x1c
  107bda:	c3                   	ret    
  107bdb:	90                   	nop
  107bdc:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]

00107be0 <sll_new>:

sllist_t *sll_new()
{
  107be0:	83 ec 18             	sub    esp,0x18
    sllist_t *list = malloc(sizeof(sllist_t));
  107be3:	6a 08                	push   0x8
  107be5:	e8 b6 f3 ff ff       	call   106fa0 <malloc>
    list->head = NULL;
  107bea:	c7 40 04 00 00 00 00 	mov    DWORD PTR [eax+0x4],0x0

    return list;
}
  107bf1:	83 c4 1c             	add    esp,0x1c
  107bf4:	c3                   	ret    
  107bf5:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
  107bf9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00107c00 <sll_display>:

void sll_display(sllist_t *list)
{
  107c00:	53                   	push   ebx
  107c01:	83 ec 08             	sub    esp,0x8
    sllist_node_t *current = list->head;
  107c04:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  107c08:	8b 58 04             	mov    ebx,DWORD PTR [eax+0x4]

    if (list->head == NULL)
  107c0b:	85 db                	test   ebx,ebx
  107c0d:	75 1e                	jne    107c2d <sll_display+0x2d>
  107c0f:	eb 35                	jmp    107c46 <sll_display+0x46>
  107c11:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
        return;

    while (current->next != NULL)
    {
        printf("%d,", current->data);
  107c18:	83 ec 08             	sub    esp,0x8
  107c1b:	ff 33                	push   DWORD PTR [ebx]
  107c1d:	68 af 24 11 00       	push   0x1124af
  107c22:	e8 49 ea ff ff       	call   106670 <printf>
        current = current->next;
  107c27:	8b 5b 04             	mov    ebx,DWORD PTR [ebx+0x4]
    while (current->next != NULL)
  107c2a:	83 c4 10             	add    esp,0x10
  107c2d:	8b 43 04             	mov    eax,DWORD PTR [ebx+0x4]
  107c30:	85 c0                	test   eax,eax
  107c32:	75 e4                	jne    107c18 <sll_display+0x18>
    }

    printf("%d\n", current->data);
  107c34:	83 ec 08             	sub    esp,0x8
  107c37:	ff 33                	push   DWORD PTR [ebx]
  107c39:	68 b3 24 11 00       	push   0x1124b3
  107c3e:	e8 2d ea ff ff       	call   106670 <printf>
  107c43:	83 c4 10             	add    esp,0x10
}
  107c46:	83 c4 08             	add    esp,0x8
  107c49:	5b                   	pop    ebx
  107c4a:	c3                   	ret    
  107c4b:	90                   	nop
  107c4c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]

00107c50 <sll_add>:

void sll_add(int data, sllist_t *list)
{
  107c50:	56                   	push   esi
  107c51:	53                   	push   ebx
  107c52:	83 ec 04             	sub    esp,0x4
  107c55:	8b 74 24 14          	mov    esi,DWORD PTR [esp+0x14]
    sllist_node_t *current = NULL;

    if (list->head == NULL)
  107c59:	8b 5e 04             	mov    ebx,DWORD PTR [esi+0x4]
  107c5c:	85 db                	test   ebx,ebx
  107c5e:	75 0a                	jne    107c6a <sll_add+0x1a>
  107c60:	eb 3e                	jmp    107ca0 <sll_add+0x50>
  107c62:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
  107c68:	89 c3                	mov    ebx,eax
    }
    else
    {
        current = list->head;

        while (current->next != NULL)
  107c6a:	8b 43 04             	mov    eax,DWORD PTR [ebx+0x4]
  107c6d:	85 c0                	test   eax,eax
  107c6f:	75 f7                	jne    107c68 <sll_add+0x18>
    sllist_node_t *newsllist_node_t = malloc(sizeof(sllist_node_t));
  107c71:	83 ec 0c             	sub    esp,0xc
  107c74:	6a 08                	push   0x8
  107c76:	e8 25 f3 ff ff       	call   106fa0 <malloc>
    newsllist_node_t->data = data;
  107c7b:	8b 54 24 20          	mov    edx,DWORD PTR [esp+0x20]
        {
            current = current->next;
        }

        current->next = createnode(data);
        list->count++;
  107c7f:	83 c4 10             	add    esp,0x10
    newsllist_node_t->next = NULL;
  107c82:	c7 40 04 00 00 00 00 	mov    DWORD PTR [eax+0x4],0x0
    newsllist_node_t->data = data;
  107c89:	89 10                	mov    DWORD PTR [eax],edx
        current->next = createnode(data);
  107c8b:	89 43 04             	mov    DWORD PTR [ebx+0x4],eax
        list->count++;
  107c8e:	83 06 01             	add    DWORD PTR [esi],0x1
    }
}
  107c91:	83 c4 04             	add    esp,0x4
  107c94:	5b                   	pop    ebx
  107c95:	5e                   	pop    esi
  107c96:	c3                   	ret    
  107c97:	89 f6                	mov    esi,esi
  107c99:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
    sllist_node_t *newsllist_node_t = malloc(sizeof(sllist_node_t));
  107ca0:	83 ec 0c             	sub    esp,0xc
  107ca3:	6a 08                	push   0x8
  107ca5:	e8 f6 f2 ff ff       	call   106fa0 <malloc>
    newsllist_node_t->data = data;
  107caa:	8b 4c 24 20          	mov    ecx,DWORD PTR [esp+0x20]
  107cae:	83 c4 10             	add    esp,0x10
    newsllist_node_t->next = NULL;
  107cb1:	c7 40 04 00 00 00 00 	mov    DWORD PTR [eax+0x4],0x0
    newsllist_node_t->data = data;
  107cb8:	89 08                	mov    DWORD PTR [eax],ecx
        list->head = createnode(data);
  107cba:	89 46 04             	mov    DWORD PTR [esi+0x4],eax
        list->count++;
  107cbd:	83 06 01             	add    DWORD PTR [esi],0x1
}
  107cc0:	83 c4 04             	add    esp,0x4
  107cc3:	5b                   	pop    ebx
  107cc4:	5e                   	pop    esi
  107cc5:	c3                   	ret    
  107cc6:	8d 76 00             	lea    esi,[esi+0x0]
  107cc9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00107cd0 <sll_delete>:

void sll_delete(int data, sllist_t *list)
{
  107cd0:	57                   	push   edi
  107cd1:	56                   	push   esi
  107cd2:	53                   	push   ebx
  107cd3:	8b 7c 24 14          	mov    edi,DWORD PTR [esp+0x14]
  107cd7:	8b 4c 24 10          	mov    ecx,DWORD PTR [esp+0x10]
    sllist_node_t *current = list->head;
  107cdb:	8b 77 04             	mov    esi,DWORD PTR [edi+0x4]
    sllist_node_t *previous = current;
    while (current != NULL)
  107cde:	85 f6                	test   esi,esi
  107ce0:	74 1d                	je     107cff <sll_delete+0x2f>
    {
        if (current->data == data)
  107ce2:	3b 0e                	cmp    ecx,DWORD PTR [esi]
  107ce4:	8b 46 04             	mov    eax,DWORD PTR [esi+0x4]
  107ce7:	74 47                	je     107d30 <sll_delete+0x60>
  107ce9:	89 f3                	mov    ebx,esi
  107ceb:	eb 0e                	jmp    107cfb <sll_delete+0x2b>
  107ced:	8d 76 00             	lea    esi,[esi+0x0]
  107cf0:	39 08                	cmp    DWORD PTR [eax],ecx
  107cf2:	8b 50 04             	mov    edx,DWORD PTR [eax+0x4]
  107cf5:	74 11                	je     107d08 <sll_delete+0x38>
  107cf7:	89 c3                	mov    ebx,eax
  107cf9:	89 d0                	mov    eax,edx
    while (current != NULL)
  107cfb:	85 c0                	test   eax,eax
  107cfd:	75 f1                	jne    107cf0 <sll_delete+0x20>
            return;
        }
        previous = current;
        current = current->next;
    }
}
  107cff:	5b                   	pop    ebx
  107d00:	5e                   	pop    esi
  107d01:	5f                   	pop    edi
  107d02:	c3                   	ret    
  107d03:	90                   	nop
  107d04:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
            if (current == list->head)
  107d08:	39 c6                	cmp    esi,eax
            previous->next = current->next;
  107d0a:	89 53 04             	mov    DWORD PTR [ebx+0x4],edx
            if (current == list->head)
  107d0d:	74 19                	je     107d28 <sll_delete+0x58>
            free(current);
  107d0f:	83 ec 0c             	sub    esp,0xc
  107d12:	50                   	push   eax
  107d13:	e8 58 f7 ff ff       	call   107470 <free>
            list->count--;
  107d18:	83 2f 01             	sub    DWORD PTR [edi],0x1
            return;
  107d1b:	83 c4 10             	add    esp,0x10
}
  107d1e:	5b                   	pop    ebx
  107d1f:	5e                   	pop    esi
  107d20:	5f                   	pop    edi
  107d21:	c3                   	ret    
  107d22:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
  107d28:	8b 50 04             	mov    edx,DWORD PTR [eax+0x4]
                list->head = current->next;
  107d2b:	89 57 04             	mov    DWORD PTR [edi+0x4],edx
  107d2e:	eb df                	jmp    107d0f <sll_delete+0x3f>
        if (current->data == data)
  107d30:	89 c2                	mov    edx,eax
  107d32:	89 f0                	mov    eax,esi
  107d34:	eb f5                	jmp    107d2b <sll_delete+0x5b>
  107d36:	8d 76 00             	lea    esi,[esi+0x0]
  107d39:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00107d40 <sll_index>:

sllist_node_t* sll_index(sllist_t *list, uint index)
{
    sllist_node_t* node = list->head;

    for(size_t i = 0; i < index - 1; i++)
  107d40:	8b 4c 24 08          	mov    ecx,DWORD PTR [esp+0x8]
    sllist_node_t* node = list->head;
  107d44:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
    for(size_t i = 0; i < index - 1; i++)
  107d48:	83 e9 01             	sub    ecx,0x1
    sllist_node_t* node = list->head;
  107d4b:	8b 40 04             	mov    eax,DWORD PTR [eax+0x4]
    for(size_t i = 0; i < index - 1; i++)
  107d4e:	74 1e                	je     107d6e <sll_index+0x2e>
  107d50:	31 d2                	xor    edx,edx
    {
        if (node != NULL)
  107d52:	85 c0                	test   eax,eax
  107d54:	75 0e                	jne    107d64 <sll_index+0x24>
  107d56:	eb 18                	jmp    107d70 <sll_index+0x30>
  107d58:	90                   	nop
  107d59:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
  107d60:	85 c0                	test   eax,eax
  107d62:	74 0a                	je     107d6e <sll_index+0x2e>
    for(size_t i = 0; i < index - 1; i++)
  107d64:	83 c2 01             	add    edx,0x1
        {
            node = node->next;
  107d67:	8b 40 04             	mov    eax,DWORD PTR [eax+0x4]
    for(size_t i = 0; i < index - 1; i++)
  107d6a:	39 ca                	cmp    edx,ecx
  107d6c:	75 f2                	jne    107d60 <sll_index+0x20>
            return NULL;
        }
    }

    return node;
}
  107d6e:	f3 c3                	repz ret 
  107d70:	f3 c3                	repz ret 
  107d72:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
  107d79:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00107d80 <sll_reverse>:

void sll_reverse(sllist_t *list)
{
  107d80:	53                   	push   ebx
  107d81:	8b 5c 24 08          	mov    ebx,DWORD PTR [esp+0x8]
    sllist_node_t *reversed = NULL;
    sllist_node_t *current = list->head;
  107d85:	8b 43 04             	mov    eax,DWORD PTR [ebx+0x4]
    sllist_node_t *temp = NULL;
    while (current != NULL)
  107d88:	85 c0                	test   eax,eax
  107d8a:	74 12                	je     107d9e <sll_reverse+0x1e>
    sllist_node_t *reversed = NULL;
  107d8c:	31 c9                	xor    ecx,ecx
  107d8e:	eb 02                	jmp    107d92 <sll_reverse+0x12>
  107d90:	89 d0                	mov    eax,edx
    {
        temp = current;
        current = current->next;
  107d92:	8b 50 04             	mov    edx,DWORD PTR [eax+0x4]
        temp->next = reversed;
  107d95:	89 48 04             	mov    DWORD PTR [eax+0x4],ecx
  107d98:	89 c1                	mov    ecx,eax
    while (current != NULL)
  107d9a:	85 d2                	test   edx,edx
  107d9c:	75 f2                	jne    107d90 <sll_reverse+0x10>
        reversed = temp;
    }
    list->head = reversed;
  107d9e:	89 43 04             	mov    DWORD PTR [ebx+0x4],eax
}
  107da1:	5b                   	pop    ebx
  107da2:	c3                   	ret    
  107da3:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
  107da9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00107db0 <sll_free>:

void sll_free(sllist_t *list)
{
  107db0:	56                   	push   esi
  107db1:	53                   	push   ebx
  107db2:	83 ec 04             	sub    esp,0x4
  107db5:	8b 74 24 10          	mov    esi,DWORD PTR [esp+0x10]
    sllist_node_t *current = list->head;
  107db9:	8b 46 04             	mov    eax,DWORD PTR [esi+0x4]
    sllist_node_t *next = current;
    while (current != NULL)
  107dbc:	85 c0                	test   eax,eax
  107dbe:	74 15                	je     107dd5 <sll_free+0x25>
    {
        next = current->next;
  107dc0:	8b 58 04             	mov    ebx,DWORD PTR [eax+0x4]
        free(current);
  107dc3:	83 ec 0c             	sub    esp,0xc
  107dc6:	50                   	push   eax
  107dc7:	e8 a4 f6 ff ff       	call   107470 <free>
    while (current != NULL)
  107dcc:	83 c4 10             	add    esp,0x10
  107dcf:	85 db                	test   ebx,ebx
  107dd1:	89 d8                	mov    eax,ebx
  107dd3:	75 eb                	jne    107dc0 <sll_free+0x10>
        current = next;
    }
    free(list);
  107dd5:	89 74 24 10          	mov    DWORD PTR [esp+0x10],esi
}
  107dd9:	83 c4 04             	add    esp,0x4
  107ddc:	5b                   	pop    ebx
  107ddd:	5e                   	pop    esi
    free(list);
  107dde:	e9 8d f6 ff ff       	jmp    107470 <free>
  107de3:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
  107de9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00107df0 <sll_destroy>:

void sll_destroy(sllist_t *list)
{
  107df0:	57                   	push   edi
  107df1:	56                   	push   esi
  107df2:	53                   	push   ebx
  107df3:	8b 7c 24 10          	mov    edi,DWORD PTR [esp+0x10]
    sllist_node_t *current = list->head;
  107df7:	8b 5f 04             	mov    ebx,DWORD PTR [edi+0x4]
    sllist_node_t *next = current;
    while (current != NULL)
  107dfa:	85 db                	test   ebx,ebx
  107dfc:	74 20                	je     107e1e <sll_destroy+0x2e>
  107dfe:	66 90                	xchg   ax,ax
    {
        next = current->next;
  107e00:	8b 73 04             	mov    esi,DWORD PTR [ebx+0x4]
        free((void *)current->data);
  107e03:	83 ec 0c             	sub    esp,0xc
  107e06:	ff 33                	push   DWORD PTR [ebx]
  107e08:	e8 63 f6 ff ff       	call   107470 <free>
        free(current);
  107e0d:	89 1c 24             	mov    DWORD PTR [esp],ebx
  107e10:	89 f3                	mov    ebx,esi
  107e12:	e8 59 f6 ff ff       	call   107470 <free>
    while (current != NULL)
  107e17:	83 c4 10             	add    esp,0x10
  107e1a:	85 f6                	test   esi,esi
  107e1c:	75 e2                	jne    107e00 <sll_destroy+0x10>
        current = next;
    }
    free(list);
  107e1e:	89 7c 24 10          	mov    DWORD PTR [esp+0x10],edi
  107e22:	5b                   	pop    ebx
  107e23:	5e                   	pop    esi
  107e24:	5f                   	pop    edi
    free(list);
  107e25:	e9 46 f6 ff ff       	jmp    107470 <free>
  107e2a:	66 90                	xchg   ax,ax
  107e2c:	66 90                	xchg   ax,ax
  107e2e:	66 90                	xchg   ax,ax

00107e30 <atomic_enable>:
bool is_atomic_enable = false;
uint ATOMIC_COUNTER = 0;

void atomic_enable()
{
    is_atomic_enable = true;
  107e30:	c6 05 64 ee 23 00 01 	mov    BYTE PTR ds:0x23ee64,0x1
}
  107e37:	c3                   	ret    
  107e38:	90                   	nop
  107e39:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]

00107e40 <atomic_begin>:

void atomic_begin()
{
    if (is_atomic_enable)
  107e40:	80 3d 64 ee 23 00 00 	cmp    BYTE PTR ds:0x23ee64,0x0
  107e47:	74 01                	je     107e4a <atomic_begin+0xa>
        cli();
  107e49:	fa                   	cli    
        ATOMIC_COUNTER++;
  107e4a:	83 05 60 ee 23 00 01 	add    DWORD PTR ds:0x23ee60,0x1
}
  107e51:	c3                   	ret    
  107e52:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
  107e59:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00107e60 <atomic_end>:

void atomic_end()
{
    ATOMIC_COUNTER--;
  107e60:	a1 60 ee 23 00       	mov    eax,ds:0x23ee60
  107e65:	83 e8 01             	sub    eax,0x1
    if (is_atomic_enable && ATOMIC_COUNTER == 0)
  107e68:	80 3d 64 ee 23 00 00 	cmp    BYTE PTR ds:0x23ee64,0x0
    ATOMIC_COUNTER--;
  107e6f:	a3 60 ee 23 00       	mov    ds:0x23ee60,eax
    if (is_atomic_enable && ATOMIC_COUNTER == 0)
  107e74:	74 04                	je     107e7a <atomic_end+0x1a>
  107e76:	85 c0                	test   eax,eax
  107e78:	74 06                	je     107e80 <atomic_end+0x20>
        sti();
  107e7a:	f3 c3                	repz ret 
  107e7c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
        sti();
  107e80:	fb                   	sti    
  107e81:	c3                   	ret    
  107e82:	66 90                	xchg   ax,ax
  107e84:	66 90                	xchg   ax,ax
  107e86:	66 90                	xchg   ax,ax
  107e88:	66 90                	xchg   ax,ax
  107e8a:	66 90                	xchg   ax,ax
  107e8c:	66 90                	xchg   ax,ax
  107e8e:	66 90                	xchg   ax,ax

00107e90 <list_item_alloc>:
#include <stdio.h>

#include "ds/list.h"

list_item_t *list_item_alloc(int value)
{
  107e90:	83 ec 18             	sub    esp,0x18
    list_item_t *item = malloc(sizeof(list_item_t));
  107e93:	6a 0c                	push   0xc
  107e95:	e8 06 f1 ff ff       	call   106fa0 <malloc>

    item->prev = NULL;
    item->next = NULL;
    item->value = value;
  107e9a:	8b 54 24 20          	mov    edx,DWORD PTR [esp+0x20]
    item->prev = NULL;
  107e9e:	c7 00 00 00 00 00    	mov    DWORD PTR [eax],0x0
    item->next = NULL;
  107ea4:	c7 40 04 00 00 00 00 	mov    DWORD PTR [eax+0x4],0x0
    item->value = value;
  107eab:	89 50 08             	mov    DWORD PTR [eax+0x8],edx

    return item;
}
  107eae:	83 c4 1c             	add    esp,0x1c
  107eb1:	c3                   	ret    
  107eb2:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
  107eb9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00107ec0 <list_alloc>:

list_t *list_alloc()
{
  107ec0:	83 ec 18             	sub    esp,0x18
    list_t *list = malloc(sizeof(list_t));
  107ec3:	6a 0c                	push   0xc
  107ec5:	e8 d6 f0 ff ff       	call   106fa0 <malloc>

    list->count = 0;
  107eca:	c7 00 00 00 00 00    	mov    DWORD PTR [eax],0x0
    list->head = NULL;
  107ed0:	c7 40 04 00 00 00 00 	mov    DWORD PTR [eax+0x4],0x0
    list->tail = NULL;
  107ed7:	c7 40 08 00 00 00 00 	mov    DWORD PTR [eax+0x8],0x0

    return list;
}
  107ede:	83 c4 1c             	add    esp,0x1c
  107ee1:	c3                   	ret    
  107ee2:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
  107ee9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00107ef0 <list_free>:

void list_free(list_t *list)
{
  107ef0:	56                   	push   esi
  107ef1:	53                   	push   ebx
  107ef2:	83 ec 04             	sub    esp,0x4
  107ef5:	8b 74 24 10          	mov    esi,DWORD PTR [esp+0x10]
    list_item_t *current = list->head;
  107ef9:	8b 46 04             	mov    eax,DWORD PTR [esi+0x4]

    while (current)
  107efc:	85 c0                	test   eax,eax
  107efe:	74 15                	je     107f15 <list_free+0x25>
    {
        list_item_t *next = current->next;
  107f00:	8b 58 04             	mov    ebx,DWORD PTR [eax+0x4]
        free(current);
  107f03:	83 ec 0c             	sub    esp,0xc
  107f06:	50                   	push   eax
  107f07:	e8 64 f5 ff ff       	call   107470 <free>
    while (current)
  107f0c:	83 c4 10             	add    esp,0x10
  107f0f:	85 db                	test   ebx,ebx
  107f11:	89 d8                	mov    eax,ebx
  107f13:	75 eb                	jne    107f00 <list_free+0x10>
        current = next;
    }

    free(list);
  107f15:	89 74 24 10          	mov    DWORD PTR [esp+0x10],esi
}
  107f19:	83 c4 04             	add    esp,0x4
  107f1c:	5b                   	pop    ebx
  107f1d:	5e                   	pop    esi
    free(list);
  107f1e:	e9 4d f5 ff ff       	jmp    107470 <free>
  107f23:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
  107f29:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00107f30 <list_destrory>:

void list_destrory(list_t *list)
{
  107f30:	57                   	push   edi
  107f31:	56                   	push   esi
  107f32:	53                   	push   ebx
  107f33:	8b 7c 24 10          	mov    edi,DWORD PTR [esp+0x10]
    list_item_t *current = list->head;
  107f37:	8b 5f 04             	mov    ebx,DWORD PTR [edi+0x4]

    while (current)
  107f3a:	85 db                	test   ebx,ebx
  107f3c:	74 21                	je     107f5f <list_destrory+0x2f>
  107f3e:	66 90                	xchg   ax,ax
    {
        list_item_t *next = current->next;
  107f40:	8b 73 04             	mov    esi,DWORD PTR [ebx+0x4]
        free((void *)current->value);
  107f43:	83 ec 0c             	sub    esp,0xc
  107f46:	ff 73 08             	push   DWORD PTR [ebx+0x8]
  107f49:	e8 22 f5 ff ff       	call   107470 <free>
        free(current);
  107f4e:	89 1c 24             	mov    DWORD PTR [esp],ebx
  107f51:	89 f3                	mov    ebx,esi
  107f53:	e8 18 f5 ff ff       	call   107470 <free>
    while (current)
  107f58:	83 c4 10             	add    esp,0x10
  107f5b:	85 f6                	test   esi,esi
  107f5d:	75 e1                	jne    107f40 <list_destrory+0x10>
        current = next;
    }

    free(list);
  107f5f:	89 7c 24 10          	mov    DWORD PTR [esp+0x10],edi
}
  107f63:	5b                   	pop    ebx
  107f64:	5e                   	pop    esi
  107f65:	5f                   	pop    edi
    free(list);
  107f66:	e9 05 f5 ff ff       	jmp    107470 <free>
  107f6b:	90                   	nop
  107f6c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]

00107f70 <list_print>:

void list_print(list_t *list)
{
  107f70:	53                   	push   ebx
  107f71:	83 ec 10             	sub    esp,0x10
  107f74:	8b 5c 24 18          	mov    ebx,DWORD PTR [esp+0x18]
    printf("c:%d ", list->count);
  107f78:	ff 33                	push   DWORD PTR [ebx]
  107f7a:	68 b7 24 11 00       	push   0x1124b7
  107f7f:	e8 ec e6 ff ff       	call   106670 <printf>
    if (list->head) printf("h:%d", list->head->value);
  107f84:	8b 43 04             	mov    eax,DWORD PTR [ebx+0x4]
  107f87:	83 c4 10             	add    esp,0x10
  107f8a:	85 c0                	test   eax,eax
  107f8c:	0f 84 f6 00 00 00    	je     108088 <list_print+0x118>
  107f92:	83 ec 08             	sub    esp,0x8
  107f95:	ff 70 08             	push   DWORD PTR [eax+0x8]
  107f98:	68 bd 24 11 00       	push   0x1124bd
  107f9d:	e8 ce e6 ff ff       	call   106670 <printf>
    else printf("h:NULL ");

    if (list->tail) printf("t:%d",list->tail->value);
  107fa2:	8b 43 08             	mov    eax,DWORD PTR [ebx+0x8]
  107fa5:	83 c4 10             	add    esp,0x10
  107fa8:	85 c0                	test   eax,eax
  107faa:	0f 84 f3 00 00 00    	je     1080a3 <list_print+0x133>
  107fb0:	83 ec 08             	sub    esp,0x8
  107fb3:	ff 70 08             	push   DWORD PTR [eax+0x8]
  107fb6:	68 ca 24 11 00       	push   0x1124ca
  107fbb:	e8 b0 e6 ff ff       	call   106670 <printf>
  107fc0:	83 c4 10             	add    esp,0x10
    else printf("t:NULL ");

    printf("\n");
  107fc3:	83 ec 0c             	sub    esp,0xc
  107fc6:	68 f9 1f 11 00       	push   0x111ff9
  107fcb:	e8 a0 e6 ff ff       	call   106670 <printf>

    FOREACH(item, list)
  107fd0:	8b 5b 04             	mov    ebx,DWORD PTR [ebx+0x4]
  107fd3:	83 c4 10             	add    esp,0x10
  107fd6:	85 db                	test   ebx,ebx
  107fd8:	74 5d                	je     108037 <list_print+0xc7>
    {
        if (item->prev) printf("%d", item->prev->value);
  107fda:	8b 03                	mov    eax,DWORD PTR [ebx]
  107fdc:	85 c0                	test   eax,eax
  107fde:	0f 84 8c 00 00 00    	je     108070 <list_print+0x100>
  107fe4:	83 ec 08             	sub    esp,0x8
  107fe7:	ff 70 08             	push   DWORD PTR [eax+0x8]
  107fea:	68 bf 24 11 00       	push   0x1124bf
  107fef:	e8 7c e6 ff ff       	call   106670 <printf>
  107ff4:	83 c4 10             	add    esp,0x10
        else printf("-");
        printf(" %d ", item->value);
  107ff7:	83 ec 08             	sub    esp,0x8
  107ffa:	ff 73 08             	push   DWORD PTR [ebx+0x8]
  107ffd:	68 d7 24 11 00       	push   0x1124d7
  108002:	e8 69 e6 ff ff       	call   106670 <printf>
        if (item->next) printf("%d", item->next->value);
  108007:	8b 43 04             	mov    eax,DWORD PTR [ebx+0x4]
  10800a:	83 c4 10             	add    esp,0x10
  10800d:	85 c0                	test   eax,eax
  10800f:	74 3f                	je     108050 <list_print+0xe0>
  108011:	83 ec 08             	sub    esp,0x8
  108014:	ff 70 08             	push   DWORD PTR [eax+0x8]
  108017:	68 bf 24 11 00       	push   0x1124bf
  10801c:	e8 4f e6 ff ff       	call   106670 <printf>
        else printf("-");
        printf("\n");
  108021:	c7 04 24 f9 1f 11 00 	mov    DWORD PTR [esp],0x111ff9
    printf("\n");
  108028:	e8 43 e6 ff ff       	call   106670 <printf>
    FOREACH(item, list)
  10802d:	8b 5b 04             	mov    ebx,DWORD PTR [ebx+0x4]
  108030:	83 c4 10             	add    esp,0x10
  108033:	85 db                	test   ebx,ebx
  108035:	75 a3                	jne    107fda <list_print+0x6a>
    }
    printf("\n");
  108037:	c7 44 24 10 f9 1f 11 	mov    DWORD PTR [esp+0x10],0x111ff9
  10803e:	00 
}
  10803f:	83 c4 08             	add    esp,0x8
  108042:	5b                   	pop    ebx
    printf("\n");
  108043:	e9 28 e6 ff ff       	jmp    106670 <printf>
  108048:	90                   	nop
  108049:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
        else printf("-");
  108050:	83 ec 0c             	sub    esp,0xc
  108053:	68 f1 21 11 00       	push   0x1121f1
  108058:	e8 13 e6 ff ff       	call   106670 <printf>
        printf("\n");
  10805d:	c7 04 24 f9 1f 11 00 	mov    DWORD PTR [esp],0x111ff9
  108064:	e9 62 ff ff ff       	jmp    107fcb <list_print+0x5b>
  108069:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
        else printf("-");
  108070:	83 ec 0c             	sub    esp,0xc
  108073:	68 f1 21 11 00       	push   0x1121f1
  108078:	e8 f3 e5 ff ff       	call   106670 <printf>
  10807d:	83 c4 10             	add    esp,0x10
  108080:	e9 72 ff ff ff       	jmp    107ff7 <list_print+0x87>
  108085:	8d 76 00             	lea    esi,[esi+0x0]
    else printf("h:NULL ");
  108088:	83 ec 0c             	sub    esp,0xc
  10808b:	68 c2 24 11 00       	push   0x1124c2
  108090:	e8 db e5 ff ff       	call   106670 <printf>
    if (list->tail) printf("t:%d",list->tail->value);
  108095:	8b 43 08             	mov    eax,DWORD PTR [ebx+0x8]
    else printf("h:NULL ");
  108098:	83 c4 10             	add    esp,0x10
    if (list->tail) printf("t:%d",list->tail->value);
  10809b:	85 c0                	test   eax,eax
  10809d:	0f 85 0d ff ff ff    	jne    107fb0 <list_print+0x40>
    else printf("t:NULL ");
  1080a3:	83 ec 0c             	sub    esp,0xc
  1080a6:	68 cf 24 11 00       	push   0x1124cf
  1080ab:	e8 c0 e5 ff ff       	call   106670 <printf>
  1080b0:	83 c4 10             	add    esp,0x10
  1080b3:	e9 0b ff ff ff       	jmp    107fc3 <list_print+0x53>
  1080b8:	90                   	nop
  1080b9:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]

001080c0 <list_push>:

void list_push(list_t *list, int value)
{
  1080c0:	53                   	push   ebx
  1080c1:	83 ec 14             	sub    esp,0x14
  1080c4:	8b 5c 24 1c          	mov    ebx,DWORD PTR [esp+0x1c]
    list_item_t *item = malloc(sizeof(list_item_t));
  1080c8:	6a 0c                	push   0xc
  1080ca:	e8 d1 ee ff ff       	call   106fa0 <malloc>
    item->value = value;
  1080cf:	8b 54 24 24          	mov    edx,DWORD PTR [esp+0x24]
    item->prev = NULL;
  1080d3:	c7 00 00 00 00 00    	mov    DWORD PTR [eax],0x0
    list_item_t *item = list_item_alloc(value);
    list->count++;

    if (list->head == NULL)
  1080d9:	83 c4 10             	add    esp,0x10
    item->next = NULL;
  1080dc:	c7 40 04 00 00 00 00 	mov    DWORD PTR [eax+0x4],0x0
    item->value = value;
  1080e3:	89 50 08             	mov    DWORD PTR [eax+0x8],edx
    if (list->head == NULL)
  1080e6:	8b 53 04             	mov    edx,DWORD PTR [ebx+0x4]
    list->count++;
  1080e9:	83 03 01             	add    DWORD PTR [ebx],0x1
    if (list->head == NULL)
  1080ec:	85 d2                	test   edx,edx
  1080ee:	74 10                	je     108100 <list_push+0x40>
        list->head = item;
        list->tail = item;
    }
    else
    {
        list->head->prev = item;
  1080f0:	89 02                	mov    DWORD PTR [edx],eax
        item->next = list->head;
  1080f2:	89 50 04             	mov    DWORD PTR [eax+0x4],edx
        list->head = item;
  1080f5:	89 43 04             	mov    DWORD PTR [ebx+0x4],eax
    }
}
  1080f8:	83 c4 08             	add    esp,0x8
  1080fb:	5b                   	pop    ebx
  1080fc:	c3                   	ret    
  1080fd:	8d 76 00             	lea    esi,[esi+0x0]
        list->head = item;
  108100:	89 43 04             	mov    DWORD PTR [ebx+0x4],eax
        list->tail = item;
  108103:	89 43 08             	mov    DWORD PTR [ebx+0x8],eax
}
  108106:	83 c4 08             	add    esp,0x8
  108109:	5b                   	pop    ebx
  10810a:	c3                   	ret    
  10810b:	90                   	nop
  10810c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]

00108110 <list_pop>:

int list_pop(list_t *list, int *value)
{
  108110:	53                   	push   ebx
  108111:	83 ec 08             	sub    esp,0x8
  108114:	8b 54 24 10          	mov    edx,DWORD PTR [esp+0x10]
    list_item_t *item = list->head;

    if (list->count == 0)
  108118:	8b 02                	mov    eax,DWORD PTR [edx]
  10811a:	85 c0                	test   eax,eax
  10811c:	74 35                	je     108153 <list_pop+0x43>
    {
        return 0;
    }
    else if (list->count == 1)
  10811e:	83 f8 01             	cmp    eax,0x1
    list_item_t *item = list->head;
  108121:	8b 4a 04             	mov    ecx,DWORD PTR [edx+0x4]
    else if (list->count == 1)
  108124:	74 3a                	je     108160 <list_pop+0x50>
    {
        list->count = 0;
        list->head = NULL;
        list->tail = NULL;
    }
    else if (list->count > 1)
  108126:	7e 11                	jle    108139 <list_pop+0x29>
    {
        item->next->prev = NULL;
  108128:	8b 59 04             	mov    ebx,DWORD PTR [ecx+0x4]
        list->head = item->next;

        list->count--;
  10812b:	83 e8 01             	sub    eax,0x1
        item->next->prev = NULL;
  10812e:	c7 03 00 00 00 00    	mov    DWORD PTR [ebx],0x0
        list->head = item->next;
  108134:	89 5a 04             	mov    DWORD PTR [edx+0x4],ebx
        list->count--;
  108137:	89 02                	mov    DWORD PTR [edx],eax
    }

    *value = item->value;
  108139:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  10813d:	8b 51 08             	mov    edx,DWORD PTR [ecx+0x8]
    free(item);
  108140:	83 ec 0c             	sub    esp,0xc
    *value = item->value;
  108143:	89 10                	mov    DWORD PTR [eax],edx
    free(item);
  108145:	51                   	push   ecx
  108146:	e8 25 f3 ff ff       	call   107470 <free>

    return 1;
  10814b:	83 c4 10             	add    esp,0x10
  10814e:	b8 01 00 00 00       	mov    eax,0x1
}
  108153:	83 c4 08             	add    esp,0x8
  108156:	5b                   	pop    ebx
  108157:	c3                   	ret    
  108158:	90                   	nop
  108159:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
        list->count = 0;
  108160:	c7 02 00 00 00 00    	mov    DWORD PTR [edx],0x0
        list->head = NULL;
  108166:	c7 42 04 00 00 00 00 	mov    DWORD PTR [edx+0x4],0x0
        list->tail = NULL;
  10816d:	c7 42 08 00 00 00 00 	mov    DWORD PTR [edx+0x8],0x0
  108174:	eb c3                	jmp    108139 <list_pop+0x29>
  108176:	8d 76 00             	lea    esi,[esi+0x0]
  108179:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00108180 <list_pushback>:

void list_pushback(list_t *list, int value)
{
  108180:	53                   	push   ebx
  108181:	83 ec 14             	sub    esp,0x14
  108184:	8b 5c 24 1c          	mov    ebx,DWORD PTR [esp+0x1c]
    list_item_t *item = malloc(sizeof(list_item_t));
  108188:	6a 0c                	push   0xc
  10818a:	e8 11 ee ff ff       	call   106fa0 <malloc>
    item->value = value;
  10818f:	8b 54 24 24          	mov    edx,DWORD PTR [esp+0x24]
    item->prev = NULL;
  108193:	c7 00 00 00 00 00    	mov    DWORD PTR [eax],0x0
    list_item_t *item = list_item_alloc(value);
    list->count++;

    if (list->tail == NULL)
  108199:	83 c4 10             	add    esp,0x10
    item->next = NULL;
  10819c:	c7 40 04 00 00 00 00 	mov    DWORD PTR [eax+0x4],0x0
    item->value = value;
  1081a3:	89 50 08             	mov    DWORD PTR [eax+0x8],edx
    if (list->tail == NULL)
  1081a6:	8b 53 08             	mov    edx,DWORD PTR [ebx+0x8]
    list->count++;
  1081a9:	83 03 01             	add    DWORD PTR [ebx],0x1
    if (list->tail == NULL)
  1081ac:	85 d2                	test   edx,edx
  1081ae:	74 10                	je     1081c0 <list_pushback+0x40>
        list->tail = item;
        list->head = item;
    }
    else
    {
        list->tail->next = item;
  1081b0:	89 42 04             	mov    DWORD PTR [edx+0x4],eax
        item->prev = list->tail;
  1081b3:	89 10                	mov    DWORD PTR [eax],edx
        list->tail = item;
  1081b5:	89 43 08             	mov    DWORD PTR [ebx+0x8],eax
    }
}
  1081b8:	83 c4 08             	add    esp,0x8
  1081bb:	5b                   	pop    ebx
  1081bc:	c3                   	ret    
  1081bd:	8d 76 00             	lea    esi,[esi+0x0]
        list->tail = item;
  1081c0:	89 43 08             	mov    DWORD PTR [ebx+0x8],eax
        list->head = item;
  1081c3:	89 43 04             	mov    DWORD PTR [ebx+0x4],eax
}
  1081c6:	83 c4 08             	add    esp,0x8
  1081c9:	5b                   	pop    ebx
  1081ca:	c3                   	ret    
  1081cb:	90                   	nop
  1081cc:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]

001081d0 <list_popback>:

int list_popback(list_t *list, int *value)
{
  1081d0:	53                   	push   ebx
  1081d1:	83 ec 08             	sub    esp,0x8
  1081d4:	8b 54 24 10          	mov    edx,DWORD PTR [esp+0x10]
    list_item_t *item = list->tail;

    if (list->count == 0)
  1081d8:	8b 02                	mov    eax,DWORD PTR [edx]
  1081da:	85 c0                	test   eax,eax
  1081dc:	74 35                	je     108213 <list_popback+0x43>
    {
        return 0;
    }
    else if (list->count == 1)
  1081de:	83 f8 01             	cmp    eax,0x1
    list_item_t *item = list->tail;
  1081e1:	8b 4a 08             	mov    ecx,DWORD PTR [edx+0x8]
    else if (list->count == 1)
  1081e4:	74 3a                	je     108220 <list_popback+0x50>
    {
        list->count = 0;
        list->head = NULL;
        list->tail = NULL;
    }
    else if (list->count > 1)
  1081e6:	7e 11                	jle    1081f9 <list_popback+0x29>
    {
        item->prev->next = NULL;
  1081e8:	8b 19                	mov    ebx,DWORD PTR [ecx]
        list->tail = item->prev;

        list->count--;
  1081ea:	83 e8 01             	sub    eax,0x1
        item->prev->next = NULL;
  1081ed:	c7 43 04 00 00 00 00 	mov    DWORD PTR [ebx+0x4],0x0
        list->tail = item->prev;
  1081f4:	89 5a 08             	mov    DWORD PTR [edx+0x8],ebx
        list->count--;
  1081f7:	89 02                	mov    DWORD PTR [edx],eax
    }

    *value = item->value;
  1081f9:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  1081fd:	8b 51 08             	mov    edx,DWORD PTR [ecx+0x8]
    free(item);
  108200:	83 ec 0c             	sub    esp,0xc
    *value = item->value;
  108203:	89 10                	mov    DWORD PTR [eax],edx
    free(item);
  108205:	51                   	push   ecx
  108206:	e8 65 f2 ff ff       	call   107470 <free>

    return 1;
  10820b:	83 c4 10             	add    esp,0x10
  10820e:	b8 01 00 00 00       	mov    eax,0x1
}
  108213:	83 c4 08             	add    esp,0x8
  108216:	5b                   	pop    ebx
  108217:	c3                   	ret    
  108218:	90                   	nop
  108219:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
        list->count = 0;
  108220:	c7 02 00 00 00 00    	mov    DWORD PTR [edx],0x0
        list->head = NULL;
  108226:	c7 42 04 00 00 00 00 	mov    DWORD PTR [edx+0x4],0x0
        list->tail = NULL;
  10822d:	c7 42 08 00 00 00 00 	mov    DWORD PTR [edx+0x8],0x0
  108234:	eb c3                	jmp    1081f9 <list_popback+0x29>
  108236:	8d 76 00             	lea    esi,[esi+0x0]
  108239:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00108240 <list_remove>:

int list_remove(list_t *list, int value)
{
  108240:	53                   	push   ebx
  108241:	83 ec 08             	sub    esp,0x8
  108244:	8b 4c 24 10          	mov    ecx,DWORD PTR [esp+0x10]
  108248:	8b 54 24 14          	mov    edx,DWORD PTR [esp+0x14]
    FOREACH(item, list)
  10824c:	8b 41 04             	mov    eax,DWORD PTR [ecx+0x4]
  10824f:	85 c0                	test   eax,eax
  108251:	74 6d                	je     1082c0 <list_remove+0x80>
    {
        if (item->value == value)
  108253:	3b 50 08             	cmp    edx,DWORD PTR [eax+0x8]
  108256:	75 0d                	jne    108265 <list_remove+0x25>
  108258:	eb 1e                	jmp    108278 <list_remove+0x38>
  10825a:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
  108260:	39 50 08             	cmp    DWORD PTR [eax+0x8],edx
  108263:	74 13                	je     108278 <list_remove+0x38>
    FOREACH(item, list)
  108265:	8b 40 04             	mov    eax,DWORD PTR [eax+0x4]
  108268:	85 c0                	test   eax,eax
  10826a:	75 f4                	jne    108260 <list_remove+0x20>
            return 1;
        }
    }

    return 0;
}
  10826c:	83 c4 08             	add    esp,0x8
  10826f:	5b                   	pop    ebx
  108270:	c3                   	ret    
  108271:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
            if (item->prev != NULL)
  108278:	8b 18                	mov    ebx,DWORD PTR [eax]
  10827a:	8b 50 04             	mov    edx,DWORD PTR [eax+0x4]
  10827d:	85 db                	test   ebx,ebx
  10827f:	74 2f                	je     1082b0 <list_remove+0x70>
                item->prev->next = item->next;
  108281:	89 53 04             	mov    DWORD PTR [ebx+0x4],edx
  108284:	8b 50 04             	mov    edx,DWORD PTR [eax+0x4]
            if (item->next != NULL)
  108287:	85 d2                	test   edx,edx
  108289:	74 2c                	je     1082b7 <list_remove+0x77>
                item->next->prev = item->prev;
  10828b:	89 1a                	mov    DWORD PTR [edx],ebx
            list->count--;
  10828d:	83 29 01             	sub    DWORD PTR [ecx],0x1
            free(item);
  108290:	83 ec 0c             	sub    esp,0xc
  108293:	50                   	push   eax
  108294:	e8 d7 f1 ff ff       	call   107470 <free>
            return 1;
  108299:	83 c4 10             	add    esp,0x10
  10829c:	b8 01 00 00 00       	mov    eax,0x1
}
  1082a1:	83 c4 08             	add    esp,0x8
  1082a4:	5b                   	pop    ebx
  1082a5:	c3                   	ret    
  1082a6:	8d 76 00             	lea    esi,[esi+0x0]
  1082a9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
            if (item->next != NULL)
  1082b0:	85 d2                	test   edx,edx
                list->head = item->next;
  1082b2:	89 51 04             	mov    DWORD PTR [ecx+0x4],edx
            if (item->next != NULL)
  1082b5:	75 d4                	jne    10828b <list_remove+0x4b>
                list->tail = item->prev;
  1082b7:	89 59 08             	mov    DWORD PTR [ecx+0x8],ebx
  1082ba:	eb d1                	jmp    10828d <list_remove+0x4d>
  1082bc:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
    return 0;
  1082c0:	31 c0                	xor    eax,eax
  1082c2:	eb a8                	jmp    10826c <list_remove+0x2c>
  1082c4:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
  1082ca:	8d bf 00 00 00 00    	lea    edi,[edi+0x0]

001082d0 <list_containe>:

int list_containe(list_t *list, int value)
{
    FOREACH(item, list)
  1082d0:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
{
  1082d4:	8b 54 24 08          	mov    edx,DWORD PTR [esp+0x8]
    FOREACH(item, list)
  1082d8:	8b 40 04             	mov    eax,DWORD PTR [eax+0x4]
  1082db:	85 c0                	test   eax,eax
  1082dd:	74 31                	je     108310 <list_containe+0x40>
    {
        if (item->value == value)
  1082df:	3b 50 08             	cmp    edx,DWORD PTR [eax+0x8]
  1082e2:	75 11                	jne    1082f5 <list_containe+0x25>
  1082e4:	eb 1a                	jmp    108300 <list_containe+0x30>
  1082e6:	8d 76 00             	lea    esi,[esi+0x0]
  1082e9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
  1082f0:	39 50 08             	cmp    DWORD PTR [eax+0x8],edx
  1082f3:	74 0b                	je     108300 <list_containe+0x30>
    FOREACH(item, list)
  1082f5:	8b 40 04             	mov    eax,DWORD PTR [eax+0x4]
  1082f8:	85 c0                	test   eax,eax
  1082fa:	75 f4                	jne    1082f0 <list_containe+0x20>
  1082fc:	f3 c3                	repz ret 
  1082fe:	66 90                	xchg   ax,ax
        {
            return 1;
  108300:	b8 01 00 00 00       	mov    eax,0x1
        }
    }

    return 0;
  108305:	c3                   	ret    
  108306:	8d 76 00             	lea    esi,[esi+0x0]
  108309:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
    return 0;
  108310:	31 c0                	xor    eax,eax
  108312:	c3                   	ret    
  108313:	66 90                	xchg   ax,ax
  108315:	66 90                	xchg   ax,ax
  108317:	66 90                	xchg   ax,ax
  108319:	66 90                	xchg   ax,ax
  10831b:	66 90                	xchg   ax,ax
  10831d:	66 90                	xchg   ax,ax
  10831f:	90                   	nop

00108320 <path_read>:
#include <string.h>
#include "utils/path.h"

int path_read(const char * path, int index, char * buffer)
{
  108320:	57                   	push   edi
  108321:	56                   	push   esi
  108322:	53                   	push   ebx
  108323:	8b 5c 24 10          	mov    ebx,DWORD PTR [esp+0x10]
  108327:	8b 7c 24 18          	mov    edi,DWORD PTR [esp+0x18]
  10832b:	8b 74 24 14          	mov    esi,DWORD PTR [esp+0x14]
    int current_index = 0;
    buffer[0] = '\0';
  10832f:	c6 07 00             	mov    BYTE PTR [edi],0x0
    
    if (path[0] == '/') path++;
  108332:	0f be 03             	movsx  eax,BYTE PTR [ebx]
  108335:	3c 2f                	cmp    al,0x2f
  108337:	75 07                	jne    108340 <path_read+0x20>
  108339:	0f be 43 01          	movsx  eax,BYTE PTR [ebx+0x1]
  10833d:	83 c3 01             	add    ebx,0x1

    for (int i = 0; path[i]; i++)
  108340:	84 c0                	test   al,al
  108342:	74 6c                	je     1083b0 <path_read+0x90>
  108344:	83 c3 01             	add    ebx,0x1
    int current_index = 0;
  108347:	31 d2                	xor    edx,edx
  108349:	eb 1a                	jmp    108365 <path_read+0x45>
  10834b:	90                   	nop
  10834c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
            {
                strapd(buffer, c);
            }
        }

        if ( c == '/')
  108350:	3c 2f                	cmp    al,0x2f
  108352:	75 06                	jne    10835a <path_read+0x3a>
        {            
            current_index++;
  108354:	83 c2 01             	add    edx,0x1
            buffer[0] = '\0';
  108357:	c6 07 00             	mov    BYTE PTR [edi],0x0
  10835a:	83 c3 01             	add    ebx,0x1
    for (int i = 0; path[i]; i++)
  10835d:	0f be 43 ff          	movsx  eax,BYTE PTR [ebx-0x1]
  108361:	84 c0                	test   al,al
  108363:	74 22                	je     108387 <path_read+0x67>
        if (current_index == index)
  108365:	39 d6                	cmp    esi,edx
  108367:	75 e7                	jne    108350 <path_read+0x30>
            if (c == '/')
  108369:	3c 2f                	cmp    al,0x2f
  10836b:	74 2b                	je     108398 <path_read+0x78>
                strapd(buffer, c);
  10836d:	83 ec 08             	sub    esp,0x8
  108370:	83 c3 01             	add    ebx,0x1
  108373:	50                   	push   eax
  108374:	57                   	push   edi
  108375:	e8 c6 e0 ff ff       	call   106440 <strapd>
    for (int i = 0; path[i]; i++)
  10837a:	0f be 43 ff          	movsx  eax,BYTE PTR [ebx-0x1]
                strapd(buffer, c);
  10837e:	83 c4 10             	add    esp,0x10
  108381:	89 f2                	mov    edx,esi
    for (int i = 0; path[i]; i++)
  108383:	84 c0                	test   al,al
  108385:	75 de                	jne    108365 <path_read+0x45>
        }
    }
    
    return (current_index == index && strlen(buffer)) ? 1 : 0;
  108387:	39 d6                	cmp    esi,edx
  108389:	74 0d                	je     108398 <path_read+0x78>
}
  10838b:	5b                   	pop    ebx
    return (current_index == index && strlen(buffer)) ? 1 : 0;
  10838c:	31 c0                	xor    eax,eax
}
  10838e:	5e                   	pop    esi
  10838f:	5f                   	pop    edi
  108390:	c3                   	ret    
  108391:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
    return (current_index == index && strlen(buffer)) ? 1 : 0;
  108398:	83 ec 0c             	sub    esp,0xc
  10839b:	57                   	push   edi
  10839c:	e8 1f dd ff ff       	call   1060c0 <strlen>
  1083a1:	83 c4 10             	add    esp,0x10
  1083a4:	85 c0                	test   eax,eax
  1083a6:	0f 95 c0             	setne  al
}
  1083a9:	5b                   	pop    ebx
    return (current_index == index && strlen(buffer)) ? 1 : 0;
  1083aa:	0f b6 c0             	movzx  eax,al
}
  1083ad:	5e                   	pop    esi
  1083ae:	5f                   	pop    edi
  1083af:	c3                   	ret    
    int current_index = 0;
  1083b0:	31 d2                	xor    edx,edx
  1083b2:	eb d3                	jmp    108387 <path_read+0x67>
  1083b4:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
  1083ba:	8d bf 00 00 00 00    	lea    edi,[edi+0x0]

001083c0 <path_len>:

int path_len(const char * path)
{
  1083c0:	55                   	push   ebp
  1083c1:	57                   	push   edi
    char buffer[128];    
    int count;

    for (count = 0; path_read(path, count, buffer); count++) 
  1083c2:	31 ff                	xor    edi,edi
{
  1083c4:	56                   	push   esi
  1083c5:	53                   	push   ebx
  1083c6:	81 ec 8c 00 00 00    	sub    esp,0x8c
  1083cc:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
    if (path[0] == '/') path++;
  1083d0:	8b 84 24 a0 00 00 00 	mov    eax,DWORD PTR [esp+0xa0]
    buffer[0] = '\0';
  1083d7:	c6 04 24 00          	mov    BYTE PTR [esp],0x0
    if (path[0] == '/') path++;
  1083db:	8b b4 24 a0 00 00 00 	mov    esi,DWORD PTR [esp+0xa0]
  1083e2:	0f be 00             	movsx  eax,BYTE PTR [eax]
  1083e5:	3c 2f                	cmp    al,0x2f
  1083e7:	75 09                	jne    1083f2 <path_len+0x32>
  1083e9:	89 f0                	mov    eax,esi
  1083eb:	8d 76 01             	lea    esi,[esi+0x1]
  1083ee:	0f be 40 01          	movsx  eax,BYTE PTR [eax+0x1]
    int current_index = 0;
  1083f2:	31 ed                	xor    ebp,ebp
    for (int i = 0; path[i]; i++)
  1083f4:	84 c0                	test   al,al
  1083f6:	bb 01 00 00 00       	mov    ebx,0x1
  1083fb:	75 19                	jne    108416 <path_len+0x56>
  1083fd:	eb 38                	jmp    108437 <path_len+0x77>
  1083ff:	90                   	nop
        if ( c == '/')
  108400:	3c 2f                	cmp    al,0x2f
  108402:	75 4c                	jne    108450 <path_len+0x90>
    for (int i = 0; path[i]; i++)
  108404:	0f be 04 1e          	movsx  eax,BYTE PTR [esi+ebx*1]
            current_index++;
  108408:	83 c5 01             	add    ebp,0x1
            buffer[0] = '\0';
  10840b:	c6 04 24 00          	mov    BYTE PTR [esp],0x0
    for (int i = 0; path[i]; i++)
  10840f:	84 c0                	test   al,al
  108411:	74 24                	je     108437 <path_len+0x77>
  108413:	83 c3 01             	add    ebx,0x1
        if (current_index == index)
  108416:	39 ef                	cmp    edi,ebp
  108418:	75 e6                	jne    108400 <path_len+0x40>
            if (c == '/')
  10841a:	3c 2f                	cmp    al,0x2f
  10841c:	74 4a                	je     108468 <path_len+0xa8>
                strapd(buffer, c);
  10841e:	83 ec 08             	sub    esp,0x8
  108421:	50                   	push   eax
  108422:	8d 44 24 0c          	lea    eax,[esp+0xc]
  108426:	50                   	push   eax
  108427:	e8 14 e0 ff ff       	call   106440 <strapd>
    for (int i = 0; path[i]; i++)
  10842c:	0f be 04 1e          	movsx  eax,BYTE PTR [esi+ebx*1]
                strapd(buffer, c);
  108430:	83 c4 10             	add    esp,0x10
    for (int i = 0; path[i]; i++)
  108433:	84 c0                	test   al,al
  108435:	75 dc                	jne    108413 <path_len+0x53>
    return (current_index == index && strlen(buffer)) ? 1 : 0;
  108437:	39 ef                	cmp    edi,ebp
  108439:	74 2d                	je     108468 <path_len+0xa8>
    {}

    return count;
}
  10843b:	81 c4 8c 00 00 00    	add    esp,0x8c
  108441:	89 f8                	mov    eax,edi
  108443:	5b                   	pop    ebx
  108444:	5e                   	pop    esi
  108445:	5f                   	pop    edi
  108446:	5d                   	pop    ebp
  108447:	c3                   	ret    
  108448:	90                   	nop
  108449:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
    for (int i = 0; path[i]; i++)
  108450:	0f be 04 1e          	movsx  eax,BYTE PTR [esi+ebx*1]
  108454:	84 c0                	test   al,al
  108456:	75 bb                	jne    108413 <path_len+0x53>
}
  108458:	81 c4 8c 00 00 00    	add    esp,0x8c
  10845e:	89 f8                	mov    eax,edi
  108460:	5b                   	pop    ebx
  108461:	5e                   	pop    esi
  108462:	5f                   	pop    edi
  108463:	5d                   	pop    ebp
  108464:	c3                   	ret    
  108465:	8d 76 00             	lea    esi,[esi+0x0]
    return (current_index == index && strlen(buffer)) ? 1 : 0;
  108468:	83 ec 0c             	sub    esp,0xc
  10846b:	8d 44 24 0c          	lea    eax,[esp+0xc]
  10846f:	50                   	push   eax
  108470:	e8 4b dc ff ff       	call   1060c0 <strlen>
  108475:	83 c4 10             	add    esp,0x10
  108478:	85 c0                	test   eax,eax
  10847a:	74 bf                	je     10843b <path_len+0x7b>
    for (count = 0; path_read(path, count, buffer); count++) 
  10847c:	83 c7 01             	add    edi,0x1
  10847f:	e9 4c ff ff ff       	jmp    1083d0 <path_len+0x10>
  108484:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
  10848a:	8d bf 00 00 00 00    	lea    edi,[edi+0x0]

00108490 <path_split>:

int path_split(const char * path, char * dir, char * file)
{
  108490:	55                   	push   ebp
  108491:	57                   	push   edi
  108492:	56                   	push   esi
  108493:	53                   	push   ebx
  108494:	81 ec 9c 00 00 00    	sub    esp,0x9c
    if (path == NULL || dir == NULL || file == NULL) return 0;
  10849a:	8b 9c 24 b4 00 00 00 	mov    ebx,DWORD PTR [esp+0xb4]
  1084a1:	8b b4 24 b8 00 00 00 	mov    esi,DWORD PTR [esp+0xb8]
  1084a8:	85 db                	test   ebx,ebx
  1084aa:	0f 94 c2             	sete   dl
  1084ad:	85 f6                	test   esi,esi
  1084af:	0f 94 c0             	sete   al
  1084b2:	08 c2                	or     dl,al
  1084b4:	0f 85 36 01 00 00    	jne    1085f0 <path_split+0x160>
  1084ba:	8b 8c 24 b0 00 00 00 	mov    ecx,DWORD PTR [esp+0xb0]
  1084c1:	85 c9                	test   ecx,ecx
  1084c3:	0f 84 27 01 00 00    	je     1085f0 <path_split+0x160>
    for (count = 0; path_read(path, count, buffer); count++) 
  1084c9:	31 f6                	xor    esi,esi
  1084cb:	90                   	nop
  1084cc:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
    if (path[0] == '/') path++;
  1084d0:	8b 84 24 b0 00 00 00 	mov    eax,DWORD PTR [esp+0xb0]
    buffer[0] = '\0';
  1084d7:	c6 44 24 10 00       	mov    BYTE PTR [esp+0x10],0x0
    if (path[0] == '/') path++;
  1084dc:	8b bc 24 b0 00 00 00 	mov    edi,DWORD PTR [esp+0xb0]
  1084e3:	0f be 00             	movsx  eax,BYTE PTR [eax]
  1084e6:	3c 2f                	cmp    al,0x2f
  1084e8:	75 09                	jne    1084f3 <path_split+0x63>
  1084ea:	89 f8                	mov    eax,edi
  1084ec:	8d 7f 01             	lea    edi,[edi+0x1]
  1084ef:	0f be 40 01          	movsx  eax,BYTE PTR [eax+0x1]
    int current_index = 0;
  1084f3:	31 ed                	xor    ebp,ebp
    for (int i = 0; path[i]; i++)
  1084f5:	84 c0                	test   al,al
  1084f7:	bb 01 00 00 00       	mov    ebx,0x1
  1084fc:	75 1d                	jne    10851b <path_split+0x8b>
  1084fe:	eb 40                	jmp    108540 <path_split+0xb0>
        if ( c == '/')
  108500:	3c 2f                	cmp    al,0x2f
  108502:	0f 85 f8 00 00 00    	jne    108600 <path_split+0x170>
    for (int i = 0; path[i]; i++)
  108508:	0f be 04 1f          	movsx  eax,BYTE PTR [edi+ebx*1]
            current_index++;
  10850c:	83 c5 01             	add    ebp,0x1
            buffer[0] = '\0';
  10850f:	c6 44 24 10 00       	mov    BYTE PTR [esp+0x10],0x0
    for (int i = 0; path[i]; i++)
  108514:	84 c0                	test   al,al
  108516:	74 28                	je     108540 <path_split+0xb0>
  108518:	83 c3 01             	add    ebx,0x1
        if (current_index == index)
  10851b:	39 ee                	cmp    esi,ebp
  10851d:	75 e1                	jne    108500 <path_split+0x70>
            if (c == '/')
  10851f:	3c 2f                	cmp    al,0x2f
  108521:	0f 84 09 01 00 00    	je     108630 <path_split+0x1a0>
                strapd(buffer, c);
  108527:	83 ec 08             	sub    esp,0x8
  10852a:	50                   	push   eax
  10852b:	8d 44 24 1c          	lea    eax,[esp+0x1c]
  10852f:	50                   	push   eax
  108530:	e8 0b df ff ff       	call   106440 <strapd>
    for (int i = 0; path[i]; i++)
  108535:	0f be 04 1f          	movsx  eax,BYTE PTR [edi+ebx*1]
                strapd(buffer, c);
  108539:	83 c4 10             	add    esp,0x10
    for (int i = 0; path[i]; i++)
  10853c:	84 c0                	test   al,al
  10853e:	75 d8                	jne    108518 <path_split+0x88>
    return (current_index == index && strlen(buffer)) ? 1 : 0;
  108540:	39 ee                	cmp    esi,ebp
  108542:	0f 84 e8 00 00 00    	je     108630 <path_split+0x1a0>

    int pathlen = path_len(path);

    dir[0] = '\0';
  108548:	8b 84 24 b4 00 00 00 	mov    eax,DWORD PTR [esp+0xb4]
    file[0] = '\0';

    char buffer[128];  

    for (int i = 0; path_read(path, i, buffer); i++) 
  10854f:	31 ff                	xor    edi,edi
  108551:	89 74 24 0c          	mov    DWORD PTR [esp+0xc],esi
  108555:	89 fd                	mov    ebp,edi
    dir[0] = '\0';
  108557:	c6 00 00             	mov    BYTE PTR [eax],0x0
    file[0] = '\0';
  10855a:	8b 84 24 b8 00 00 00 	mov    eax,DWORD PTR [esp+0xb8]
  108561:	c6 00 00             	mov    BYTE PTR [eax],0x0
    if (path[0] == '/') path++;
  108564:	8b 84 24 b0 00 00 00 	mov    eax,DWORD PTR [esp+0xb0]
    buffer[0] = '\0';
  10856b:	c6 44 24 10 00       	mov    BYTE PTR [esp+0x10],0x0
    if (path[0] == '/') path++;
  108570:	8b b4 24 b0 00 00 00 	mov    esi,DWORD PTR [esp+0xb0]
  108577:	0f be 00             	movsx  eax,BYTE PTR [eax]
  10857a:	3c 2f                	cmp    al,0x2f
  10857c:	75 09                	jne    108587 <path_split+0xf7>
  10857e:	89 f0                	mov    eax,esi
  108580:	8d 76 01             	lea    esi,[esi+0x1]
  108583:	0f be 40 01          	movsx  eax,BYTE PTR [eax+0x1]
    int current_index = 0;
  108587:	31 ff                	xor    edi,edi
    for (int i = 0; path[i]; i++)
  108589:	84 c0                	test   al,al
  10858b:	bb 01 00 00 00       	mov    ebx,0x1
  108590:	75 1d                	jne    1085af <path_split+0x11f>
  108592:	eb 40                	jmp    1085d4 <path_split+0x144>
  108594:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
        if ( c == '/')
  108598:	3c 2f                	cmp    al,0x2f
  10859a:	75 7c                	jne    108618 <path_split+0x188>
    for (int i = 0; path[i]; i++)
  10859c:	0f be 04 1e          	movsx  eax,BYTE PTR [esi+ebx*1]
            current_index++;
  1085a0:	83 c7 01             	add    edi,0x1
            buffer[0] = '\0';
  1085a3:	c6 44 24 10 00       	mov    BYTE PTR [esp+0x10],0x0
    for (int i = 0; path[i]; i++)
  1085a8:	84 c0                	test   al,al
  1085aa:	74 28                	je     1085d4 <path_split+0x144>
  1085ac:	83 c3 01             	add    ebx,0x1
        if (current_index == index)
  1085af:	39 fd                	cmp    ebp,edi
  1085b1:	75 e5                	jne    108598 <path_split+0x108>
            if (c == '/')
  1085b3:	3c 2f                	cmp    al,0x2f
  1085b5:	0f 84 95 00 00 00    	je     108650 <path_split+0x1c0>
                strapd(buffer, c);
  1085bb:	83 ec 08             	sub    esp,0x8
  1085be:	50                   	push   eax
  1085bf:	8d 44 24 1c          	lea    eax,[esp+0x1c]
  1085c3:	50                   	push   eax
  1085c4:	e8 77 de ff ff       	call   106440 <strapd>
    for (int i = 0; path[i]; i++)
  1085c9:	0f be 04 1e          	movsx  eax,BYTE PTR [esi+ebx*1]
                strapd(buffer, c);
  1085cd:	83 c4 10             	add    esp,0x10
    for (int i = 0; path[i]; i++)
  1085d0:	84 c0                	test   al,al
  1085d2:	75 d8                	jne    1085ac <path_split+0x11c>
    return (current_index == index && strlen(buffer)) ? 1 : 0;
  1085d4:	39 fd                	cmp    ebp,edi
  1085d6:	74 78                	je     108650 <path_split+0x1c0>
        {
            strcpy(file, buffer);
        }
    }

    return 1;
  1085d8:	b8 01 00 00 00       	mov    eax,0x1
  1085dd:	81 c4 9c 00 00 00    	add    esp,0x9c
  1085e3:	5b                   	pop    ebx
  1085e4:	5e                   	pop    esi
  1085e5:	5f                   	pop    edi
  1085e6:	5d                   	pop    ebp
  1085e7:	c3                   	ret    
  1085e8:	90                   	nop
  1085e9:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
  1085f0:	81 c4 9c 00 00 00    	add    esp,0x9c
    if (path == NULL || dir == NULL || file == NULL) return 0;
  1085f6:	31 c0                	xor    eax,eax
  1085f8:	5b                   	pop    ebx
  1085f9:	5e                   	pop    esi
  1085fa:	5f                   	pop    edi
  1085fb:	5d                   	pop    ebp
  1085fc:	c3                   	ret    
  1085fd:	8d 76 00             	lea    esi,[esi+0x0]
    for (int i = 0; path[i]; i++)
  108600:	0f be 04 1f          	movsx  eax,BYTE PTR [edi+ebx*1]
  108604:	84 c0                	test   al,al
  108606:	0f 85 0c ff ff ff    	jne    108518 <path_split+0x88>
  10860c:	e9 37 ff ff ff       	jmp    108548 <path_split+0xb8>
  108611:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
  108618:	0f be 04 1e          	movsx  eax,BYTE PTR [esi+ebx*1]
  10861c:	84 c0                	test   al,al
  10861e:	75 8c                	jne    1085ac <path_split+0x11c>
    return 1;
  108620:	b8 01 00 00 00       	mov    eax,0x1
  108625:	eb b6                	jmp    1085dd <path_split+0x14d>
  108627:	89 f6                	mov    esi,esi
  108629:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
    return (current_index == index && strlen(buffer)) ? 1 : 0;
  108630:	83 ec 0c             	sub    esp,0xc
  108633:	8d 44 24 1c          	lea    eax,[esp+0x1c]
  108637:	50                   	push   eax
  108638:	e8 83 da ff ff       	call   1060c0 <strlen>
  10863d:	83 c4 10             	add    esp,0x10
  108640:	85 c0                	test   eax,eax
  108642:	0f 84 00 ff ff ff    	je     108548 <path_split+0xb8>
    for (count = 0; path_read(path, count, buffer); count++) 
  108648:	83 c6 01             	add    esi,0x1
  10864b:	e9 80 fe ff ff       	jmp    1084d0 <path_split+0x40>
    return (current_index == index && strlen(buffer)) ? 1 : 0;
  108650:	83 ec 0c             	sub    esp,0xc
  108653:	8d 44 24 1c          	lea    eax,[esp+0x1c]
  108657:	50                   	push   eax
  108658:	e8 63 da ff ff       	call   1060c0 <strlen>
  10865d:	83 c4 10             	add    esp,0x10
  108660:	85 c0                	test   eax,eax
  108662:	0f 84 70 ff ff ff    	je     1085d8 <path_split+0x148>
        if (i < pathlen - 1)
  108668:	8b 44 24 0c          	mov    eax,DWORD PTR [esp+0xc]
  10866c:	83 e8 01             	sub    eax,0x1
  10866f:	39 e8                	cmp    eax,ebp
  108671:	7e 35                	jle    1086a8 <path_split+0x218>
            strcat(dir, buffer);
  108673:	83 ec 08             	sub    esp,0x8
  108676:	8d 44 24 18          	lea    eax,[esp+0x18]
  10867a:	50                   	push   eax
  10867b:	ff b4 24 c0 00 00 00 	push   DWORD PTR [esp+0xc0]
  108682:	e8 59 d2 ff ff       	call   1058e0 <strcat>
            strapd(dir, '/');
  108687:	58                   	pop    eax
  108688:	5a                   	pop    edx
  108689:	6a 2f                	push   0x2f
  10868b:	ff b4 24 c0 00 00 00 	push   DWORD PTR [esp+0xc0]
  108692:	e8 a9 dd ff ff       	call   106440 <strapd>
  108697:	83 c4 10             	add    esp,0x10
    for (int i = 0; path_read(path, i, buffer); i++) 
  10869a:	83 c5 01             	add    ebp,0x1
  10869d:	e9 c2 fe ff ff       	jmp    108564 <path_split+0xd4>
  1086a2:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
            strcpy(file, buffer);
  1086a8:	83 ec 08             	sub    esp,0x8
  1086ab:	8d 44 24 18          	lea    eax,[esp+0x18]
  1086af:	50                   	push   eax
  1086b0:	ff b4 24 c4 00 00 00 	push   DWORD PTR [esp+0xc4]
  1086b7:	e8 54 d7 ff ff       	call   105e10 <strcpy>
  1086bc:	83 c4 10             	add    esp,0x10
  1086bf:	eb d9                	jmp    10869a <path_split+0x20a>
  1086c1:	66 90                	xchg   ax,ax
  1086c3:	66 90                	xchg   ax,ax
  1086c5:	66 90                	xchg   ax,ax
  1086c7:	66 90                	xchg   ax,ax
  1086c9:	66 90                	xchg   ax,ax
  1086cb:	66 90                	xchg   ax,ax
  1086cd:	66 90                	xchg   ax,ax
  1086cf:	90                   	nop

001086d0 <check_magic>:
#include <string.h>
#include "libelf.h"

int check_magic(ELF_header_t *header)
{
  1086d0:	8b 54 24 04          	mov    edx,DWORD PTR [esp+0x4]

    return magic[0] == ELFMAG0 &&
           magic[1] == ELFMAG1 &&
           magic[2] == ELFMAG2 &&
           magic[3] == ELFMAG3 &&
           magic[4] == 1 && /* is 32bit */
  1086d4:	31 c0                	xor    eax,eax
  1086d6:	80 3a 7f             	cmp    BYTE PTR [edx],0x7f
  1086d9:	74 05                	je     1086e0 <check_magic+0x10>
           magic[5] == 1;
}
  1086db:	f3 c3                	repz ret 
  1086dd:	8d 76 00             	lea    esi,[esi+0x0]
    return magic[0] == ELFMAG0 &&
  1086e0:	80 7a 01 45          	cmp    BYTE PTR [edx+0x1],0x45
  1086e4:	75 f5                	jne    1086db <check_magic+0xb>
           magic[1] == ELFMAG1 &&
  1086e6:	80 7a 02 4c          	cmp    BYTE PTR [edx+0x2],0x4c
  1086ea:	75 ef                	jne    1086db <check_magic+0xb>
           magic[2] == ELFMAG2 &&
  1086ec:	80 7a 03 46          	cmp    BYTE PTR [edx+0x3],0x46
  1086f0:	75 e9                	jne    1086db <check_magic+0xb>
           magic[3] == ELFMAG3 &&
  1086f2:	80 7a 04 01          	cmp    BYTE PTR [edx+0x4],0x1
  1086f6:	75 e3                	jne    1086db <check_magic+0xb>
           magic[4] == 1 && /* is 32bit */
  1086f8:	31 c0                	xor    eax,eax
  1086fa:	80 7a 05 01          	cmp    BYTE PTR [edx+0x5],0x1
  1086fe:	0f 94 c0             	sete   al
}
  108701:	c3                   	ret    
  108702:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
  108709:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00108710 <ELF_valid>:

int ELF_valid(ELF_header_t *header)
{
  108710:	8b 54 24 04          	mov    edx,DWORD PTR [esp+0x4]
    return check_magic(header) &&
           (header->type == ELF_REL || ELF_EXEC) &&
           header->version == 1 &&
  108714:	31 c0                	xor    eax,eax
           magic[4] == 1 && /* is 32bit */
  108716:	80 3a 7f             	cmp    BYTE PTR [edx],0x7f
  108719:	74 05                	je     108720 <ELF_valid+0x10>
           header->machine == 3; // 386
}
  10871b:	f3 c3                	repz ret 
  10871d:	8d 76 00             	lea    esi,[esi+0x0]
    return magic[0] == ELFMAG0 &&
  108720:	80 7a 01 45          	cmp    BYTE PTR [edx+0x1],0x45
  108724:	75 f5                	jne    10871b <ELF_valid+0xb>
           magic[1] == ELFMAG1 &&
  108726:	80 7a 02 4c          	cmp    BYTE PTR [edx+0x2],0x4c
  10872a:	75 ef                	jne    10871b <ELF_valid+0xb>
           magic[2] == ELFMAG2 &&
  10872c:	80 7a 03 46          	cmp    BYTE PTR [edx+0x3],0x46
  108730:	75 e9                	jne    10871b <ELF_valid+0xb>
           magic[3] == ELFMAG3 &&
  108732:	80 7a 04 01          	cmp    BYTE PTR [edx+0x4],0x1
  108736:	75 e3                	jne    10871b <ELF_valid+0xb>
           magic[4] == 1 && /* is 32bit */
  108738:	80 7a 05 01          	cmp    BYTE PTR [edx+0x5],0x1
  10873c:	75 dd                	jne    10871b <ELF_valid+0xb>
           (header->type == ELF_REL || ELF_EXEC) &&
  10873e:	83 7a 14 01          	cmp    DWORD PTR [edx+0x14],0x1
  108742:	75 d7                	jne    10871b <ELF_valid+0xb>
           header->version == 1 &&
  108744:	31 c0                	xor    eax,eax
  108746:	66 83 7a 12 03       	cmp    WORD PTR [edx+0x12],0x3
  10874b:	0f 94 c0             	sete   al
}
  10874e:	c3                   	ret    
  10874f:	90                   	nop

00108750 <ELF_read_section>:

int ELF_read_section(ELF_header_t *header, ELF_section_t *dest, uint index)
{
  108750:	53                   	push   ebx
  108751:	31 c0                	xor    eax,eax
  108753:	83 ec 08             	sub    esp,0x8
  108756:	8b 54 24 10          	mov    edx,DWORD PTR [esp+0x10]
  10875a:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
    if (index >= header->shnum)
  10875e:	0f b7 5a 30          	movzx  ebx,WORD PTR [edx+0x30]
  108762:	39 cb                	cmp    ebx,ecx
  108764:	76 20                	jbe    108786 <ELF_read_section+0x36>
        return 0;
    ELF_section_t *section = (ELF_section_t *)((uint)header + header->shoff + sizeof(ELF_section_t) * index);
  108766:	03 52 20             	add    edx,DWORD PTR [edx+0x20]
  108769:	8d 04 89             	lea    eax,[ecx+ecx*4]

    memcpy(dest, section, sizeof(ELF_section_t));
  10876c:	83 ec 04             	sub    esp,0x4
  10876f:	6a 28                	push   0x28
    ELF_section_t *section = (ELF_section_t *)((uint)header + header->shoff + sizeof(ELF_section_t) * index);
  108771:	8d 04 c2             	lea    eax,[edx+eax*8]
    memcpy(dest, section, sizeof(ELF_section_t));
  108774:	50                   	push   eax
  108775:	ff 74 24 20          	push   DWORD PTR [esp+0x20]
  108779:	e8 92 ce ff ff       	call   105610 <memcpy>
  10877e:	83 c4 10             	add    esp,0x10
  108781:	b8 01 00 00 00       	mov    eax,0x1

    return 1;
}
  108786:	83 c4 08             	add    esp,0x8
  108789:	5b                   	pop    ebx
  10878a:	c3                   	ret    
  10878b:	90                   	nop
  10878c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]

00108790 <ELF_lookup_string>:

char *ELF_lookup_string(ELF_header_t *header, int offset)
{
  108790:	53                   	push   ebx
  108791:	83 ec 38             	sub    esp,0x38
  108794:	8b 5c 24 40          	mov    ebx,DWORD PTR [esp+0x40]
    if (index >= header->shnum)
  108798:	0f b7 4b 32          	movzx  ecx,WORD PTR [ebx+0x32]
  10879c:	0f b7 43 32          	movzx  eax,WORD PTR [ebx+0x32]
  1087a0:	66 39 4b 30          	cmp    WORD PTR [ebx+0x30],cx
  1087a4:	76 1e                	jbe    1087c4 <ELF_lookup_string+0x34>
    ELF_section_t *section = (ELF_section_t *)((uint)header + header->shoff + sizeof(ELF_section_t) * index);
  1087a6:	8b 53 20             	mov    edx,DWORD PTR [ebx+0x20]
  1087a9:	8d 04 80             	lea    eax,[eax+eax*4]
    memcpy(dest, section, sizeof(ELF_section_t));
  1087ac:	83 ec 04             	sub    esp,0x4
  1087af:	6a 28                	push   0x28
    ELF_section_t *section = (ELF_section_t *)((uint)header + header->shoff + sizeof(ELF_section_t) * index);
  1087b1:	01 da                	add    edx,ebx
  1087b3:	8d 04 c2             	lea    eax,[edx+eax*8]
    memcpy(dest, section, sizeof(ELF_section_t));
  1087b6:	50                   	push   eax
  1087b7:	8d 44 24 14          	lea    eax,[esp+0x14]
  1087bb:	50                   	push   eax
  1087bc:	e8 4f ce ff ff       	call   105610 <memcpy>
  1087c1:	83 c4 10             	add    esp,0x10
    ELF_section_t section;
    ELF_read_section(header, &section, header->shstrndx);
    return (char *)header + section.offset + offset;
  1087c4:	8b 44 24 44          	mov    eax,DWORD PTR [esp+0x44]
  1087c8:	03 44 24 18          	add    eax,DWORD PTR [esp+0x18]
}
  1087cc:	83 c4 38             	add    esp,0x38
    return (char *)header + section.offset + offset;
  1087cf:	01 d8                	add    eax,ebx
}
  1087d1:	5b                   	pop    ebx
  1087d2:	c3                   	ret    
  1087d3:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
  1087d9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

001087e0 <ELF_read_program>:

int ELF_read_program(ELF_header_t *header, ELF_program_t *dest, uint index)
{
  1087e0:	53                   	push   ebx
  1087e1:	31 c0                	xor    eax,eax
  1087e3:	83 ec 08             	sub    esp,0x8
  1087e6:	8b 54 24 10          	mov    edx,DWORD PTR [esp+0x10]
  1087ea:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
    if (index >= header->phnum)
  1087ee:	0f b7 5a 2c          	movzx  ebx,WORD PTR [edx+0x2c]
  1087f2:	39 cb                	cmp    ebx,ecx
  1087f4:	76 22                	jbe    108818 <ELF_read_program+0x38>
        return 0;
        
    ELF_program_t *section = (ELF_program_t *)((uint)header + header->phoff + sizeof(ELF_program_t) * index);
  1087f6:	8b 42 1c             	mov    eax,DWORD PTR [edx+0x1c]
  1087f9:	c1 e1 05             	shl    ecx,0x5

    memcpy(dest, section, sizeof(ELF_program_t));
  1087fc:	83 ec 04             	sub    esp,0x4
  1087ff:	6a 20                	push   0x20
    ELF_program_t *section = (ELF_program_t *)((uint)header + header->phoff + sizeof(ELF_program_t) * index);
  108801:	01 d0                	add    eax,edx
  108803:	8d 14 08             	lea    edx,[eax+ecx*1]
    memcpy(dest, section, sizeof(ELF_program_t));
  108806:	52                   	push   edx
  108807:	ff 74 24 20          	push   DWORD PTR [esp+0x20]
  10880b:	e8 00 ce ff ff       	call   105610 <memcpy>

    return 1;
  108810:	83 c4 10             	add    esp,0x10
  108813:	b8 01 00 00 00       	mov    eax,0x1
  108818:	83 c4 08             	add    esp,0x8
  10881b:	5b                   	pop    ebx
  10881c:	c3                   	ret    

Déassemblage de la section .text.startup :

00108820 <main>:

int exec(char *path);
void main(multiboot_info_t *info, s32 magic)
{
  108820:	8d 4c 24 04          	lea    ecx,[esp+0x4]
  108824:	83 e4 f0             	and    esp,0xfffffff0
  108827:	ff 71 fc             	push   DWORD PTR [ecx-0x4]
  10882a:	55                   	push   ebp
  10882b:	89 e5                	mov    ebp,esp
  10882d:	56                   	push   esi
  10882e:	53                   	push   ebx
  10882f:	51                   	push   ecx
  108830:	83 ec 18             	sub    esp,0x18
  108833:	8b 31                	mov    esi,DWORD PTR [ecx]
  108835:	8b 59 04             	mov    ebx,DWORD PTR [ecx+0x4]
    puts("\n");
  108838:	68 f9 1f 11 00       	push   0x111ff9
  10883d:	e8 5e f0 ff ff       	call   1078a0 <puts>
    boot_screen("Booting...");
  108842:	c7 04 24 38 21 11 00 	mov    DWORD PTR [esp],0x112138
  108849:	e8 e2 ad ff ff       	call   103630 <boot_screen>

    memcpy(&mbootinfo, info, sizeof(multiboot_info_t));
  10884e:	83 c4 0c             	add    esp,0xc
  108851:	6a 78                	push   0x78
  108853:	56                   	push   esi
  108854:	68 00 a0 23 00       	push   0x23a000
  108859:	e8 b2 cd ff ff       	call   105610 <memcpy>

    if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
  10885e:	83 c4 10             	add    esp,0x10
  108861:	81 fb 02 b0 ad 2b    	cmp    ebx,0x2badb002
  108867:	74 1e                	je     108887 <main+0x67>
        PANIC("Invalid multiboot magic number (0x%x)!", magic);
  108869:	50                   	push   eax
  10886a:	50                   	push   eax
  10886b:	53                   	push   ebx
  10886c:	68 90 66 11 00       	push   0x116690
  108871:	6a 00                	push   0x0
  108873:	6a 35                	push   0x35
  108875:	68 bc 1d 11 00       	push   0x111dbc
  10887a:	68 43 21 11 00       	push   0x112143
  10887f:	e8 2c 98 ff ff       	call   1020b0 <__panic>
  108884:	83 c4 20             	add    esp,0x20

    log("--- Setting up cpu tables ---");
  108887:	83 ec 0c             	sub    esp,0xc
  10888a:	68 61 21 11 00       	push   0x112161
  10888f:	e8 0c 8e ff ff       	call   1016a0 <log>
    setup(gdt);
  108894:	c7 04 24 7f 21 11 00 	mov    DWORD PTR [esp],0x11217f
  10889b:	e8 00 8e ff ff       	call   1016a0 <log>
  1088a0:	e8 0b be ff ff       	call   1046b0 <gdt_setup>
    setup(pic);
  1088a5:	c7 04 24 91 21 11 00 	mov    DWORD PTR [esp],0x112191
  1088ac:	e8 ef 8d ff ff       	call   1016a0 <log>
  1088b1:	e8 da c3 ff ff       	call   104c90 <pic_setup>
    setup(idt);
  1088b6:	c7 04 24 a3 21 11 00 	mov    DWORD PTR [esp],0x1121a3
  1088bd:	e8 de 8d ff ff       	call   1016a0 <log>
  1088c2:	e8 49 c2 ff ff       	call   104b10 <idt_setup>
    setup(isr);
  1088c7:	c7 04 24 b5 21 11 00 	mov    DWORD PTR [esp],0x1121b5
  1088ce:	e8 cd 8d ff ff       	call   1016a0 <log>
  1088d3:	e8 c8 c2 ff ff       	call   104ba0 <isr_setup>
    setup(irq);
  1088d8:	c7 04 24 c7 21 11 00 	mov    DWORD PTR [esp],0x1121c7
  1088df:	e8 bc 8d ff ff       	call   1016a0 <log>
  1088e4:	e8 57 ba ff ff       	call   104340 <irq_setup>

    log("--- Setting up system ---");
  1088e9:	c7 04 24 d9 21 11 00 	mov    DWORD PTR [esp],0x1121d9
  1088f0:	e8 ab 8d ff ff       	call   1016a0 <log>
    setup(memory, get_kernel_end(&mbootinfo), (mbootinfo.mem_lower + mbootinfo.mem_upper) * 1024);
  1088f5:	c7 04 24 f3 21 11 00 	mov    DWORD PTR [esp],0x1121f3
  1088fc:	e8 9f 8d ff ff       	call   1016a0 <log>
  108901:	8b 1d 08 a0 23 00    	mov    ebx,DWORD PTR ds:0x23a008
  108907:	03 1d 04 a0 23 00    	add    ebx,DWORD PTR ds:0x23a004
  10890d:	c7 04 24 00 a0 23 00 	mov    DWORD PTR [esp],0x23a000
  108914:	e8 77 ad ff ff       	call   103690 <get_kernel_end>
  108919:	5a                   	pop    edx
  10891a:	59                   	pop    ecx
  10891b:	c1 e3 0a             	shl    ebx,0xa
  10891e:	53                   	push   ebx
  10891f:	50                   	push   eax
  108920:	e8 3b a2 ff ff       	call   102b60 <memory_setup>
    setup(tasking);
  108925:	c7 04 24 08 22 11 00 	mov    DWORD PTR [esp],0x112208
  10892c:	e8 6f 8d ff ff       	call   1016a0 <log>
  108931:	e8 5a 90 ff ff       	call   101990 <tasking_setup>
    setup(filesystem);
  108936:	c7 04 24 1e 22 11 00 	mov    DWORD PTR [esp],0x11221e
  10893d:	e8 5e 8d ff ff       	call   1016a0 <log>
  108942:	e8 c9 76 ff ff       	call   100010 <filesystem_setup>
    setup(modules, &mbootinfo);
  108947:	c7 04 24 37 22 11 00 	mov    DWORD PTR [esp],0x112237
  10894e:	e8 4d 8d ff ff       	call   1016a0 <log>
  108953:	c7 04 24 00 a0 23 00 	mov    DWORD PTR [esp],0x23a000
  10895a:	e8 a1 8a ff ff       	call   101400 <modules_setup>

    boot_screen("Welcome!");
  10895f:	c7 04 24 4d 22 11 00 	mov    DWORD PTR [esp],0x11224d
  108966:	e8 c5 ac ff ff       	call   103630 <boot_screen>
    atomic_enable();
  10896b:	e8 c0 f4 ff ff       	call   107e30 <atomic_enable>
    sti();
  108970:	fb                   	sti    

    thread_create(time_task);
  108971:	c7 04 24 c0 4f 10 00 	mov    DWORD PTR [esp],0x104fc0
  108978:	e8 23 91 ff ff       	call   101aa0 <thread_create>
    log(KERNEL_UNAME);
  10897d:	5b                   	pop    ebx
  10897e:	5e                   	pop    esi
  10897f:	ff 35 08 70 11 00    	push   DWORD PTR ds:0x117008
  108985:	ff 35 14 ae 23 00    	push   DWORD PTR ds:0x23ae14
  10898b:	ff 35 0c 70 11 00    	push   DWORD PTR ds:0x11700c
  108991:	ff 35 18 ae 23 00    	push   DWORD PTR ds:0x23ae18
  108997:	ff 35 10 70 11 00    	push   DWORD PTR ds:0x117010
  10899d:	ff 35 00 70 11 00    	push   DWORD PTR ds:0x117000
  1089a3:	e8 f8 8c ff ff       	call   1016a0 <log>

    process_exec("application/test-app.app", 0, NULL);
  1089a8:	83 c4 1c             	add    esp,0x1c
  1089ab:	6a 00                	push   0x0
  1089ad:	6a 00                	push   0x0
  1089af:	68 56 22 11 00       	push   0x112256
  1089b4:	e8 57 93 ff ff       	call   101d10 <process_exec>
    //exec("Application/test-app.app");

    log("kernel running");
  1089b9:	c7 04 24 6f 22 11 00 	mov    DWORD PTR [esp],0x11226f
  1089c0:	e8 db 8c ff ff       	call   1016a0 <log>
  1089c5:	83 c4 10             	add    esp,0x10
  1089c8:	90                   	nop
  1089c9:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]

    while (true)
    {
        hlt();
  1089d0:	f4                   	hlt    
  1089d1:	eb fd                	jmp    1089d0 <main+0x1b0>
