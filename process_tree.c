#include <unistd.h> // getpid() fonksiyonu için
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

/* ************************************************************************
Her parent'ın 2 tane child'ı var. Leafler sadece dosya oluşturdular ve oluşturdukları dosyaların
içine rastgele int sayı yazdılar. Parentlarsa child'ların pidleriyle verilmiş isimli dosyaları
açıyor, içlerindeki sayıları okuyor ve toplayıp üçüncü bir dosyaya kendi pid'siyle olan bir dosyaya yazıyor.
**************************************************************************** */

//yaprakların yapacağı iş -> dosya açacaklar
void yaprak()
{
	char dosya_isim[20];
	sprintf(dosya_isim, "%d.txt", getpid()); 
	/*
		yaprak fonk çalıştığı zaman çalışan processin pid'sini alacak,
		aldığı pid'yi dosyanın içine yazacak, dosya_isim olarak yazdıracak
	*/
	FILE *fp = fopen(dosya_isim, "w"); 
	/*	-----------
		write modunda dosyayı açıyoruz çünkü dosyaların içine rastgele sayı yazacak
	---------- */
	srand(getpid());
	/*
	srand(time(NULL)); yapamama sebebimiz tüm processler aynı zamanı alacağından dolayı
	hepsi aynı rastgele sayıyı üretmiş olacağından dolayı
	*/
	fprintf(fp, "%d", (rand()%100)); //dosyaya (yani fp'ye) iki haneyi geçmeyen sayıları yaz
	fclose(fp);
}

int parent(int pid1, int pid2) //parent kendi child'larının dosyasını açacağı için sol-sağ childlarının idlerini geçirsin
{
	wait(NULL);
	/*iki tane processin dosyalarını açacak ve onları okuyacak*/
	char dosya_isim1[20];
	sprintf(dosya_isim1, "%d.txt", pid1);
	char dosya_isim2[20];
	sprintf(dosya_isim2, "%d.txt", pid2); 
	char dosya_isim3[20];
	sprintf(dosya_isim3, "%d.txt", getpid()); 
	FILE *fp1, *fp2, *fp3;
	fp1 = fopen(dosya_isim1, "r");
	fp2 = fopen(dosya_isim2, "r");
	//dosyaları okumak için açtık^, bu dosyalardan birer tane int değer okuyacağız(aşağıdaki işl.)
	int sol, sag;
	fscanf(fp1, "%d", &sol); //fp1'den sol değişkeninin içini oku
	fscanf(fp2, "%d", &sag); //fp2'den sağ değişkeninin içini oku
	fclose(fp1);
	fclose(fp2);
	fp3 = fopen(dosya_isim3, "w");
	fprintf(fp3, "%d", sol+sag);	//soldan gelen int değerle sağdan gelen int değeri toplayıp fp3'e yaz
	fclose(fp3);
}

//ikili ağaç oluşturuyoruz (binary_tree)
void	agac_olustur(int yukseklik) //yukseklik 3ten başlayacak, sıfıra kadar gidecek, sıfır olunca çalışmayacak
{
	if (yukseklik == 0)	{ yaprak(); return; }
	int pid = fork();

	/* ***********************************************************
	int pid2 = fork(); yaparsak her parent bir tane child fork etmemiş olur
	yani bir soy ağacı gibi parent en üstte sonrasında dallanmalar olacak
	
	--bunun için de processin parent, child olduğu durumları şartlandırıp 
	ona göre fork işlemi yapacağız
	*************************************************************** */
	if (pid > 0)
	{
		int pid2 = fork();
		if (pid2 > 0)
		{
			//parent işlemi
			wait(NULL); //childların bitmesini garanti ediyor
			printf("parent: %d, c1:%d, c2:%d\n", getpid(), pid, pid2);
			fflush(stdout); //basılmama ihtimalinden kaynaklı bunu ekliyoruz
			//birden fazla process çalıştığı için basılma sırada bekleyip basılmayabilir
			parent(pid, pid2);
		}
		else if (pid2 == 0)
		{
			//sol çocuk
			agac_olustur(--yukseklik);
		}
	}
	else if (pid == 0)
	{
		//sağ çocuk
		agac_olustur(--yukseklik);
	}
}

int main()
{
	agac_olustur(3);
}
