#include <iostream>
#include <string>
#include <chrono>
#include <opencv2\opencv.hpp>
#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\videoio.hpp>

extern "C" {
#include <string.h>
#include "vc.h"
}



int BoundingBox(IVC* src);
void Frame(IVC* frame, char *c);
int vhistogram(int h[], int n, int kernel, int cmin);
int* GetRowProf(IVC* image, int y);
int* GetColunmProf(IVC* image);
void mediaMovel(double* histograma, int n);
double* HistogramaHorizontalF(IVC* image);
double* HistogramaVerticalF(IVC* image);
void normalizaHist(double* histograma, int n, int totalPixeis);
char IdentificaCaracter(IVC* image);
int* IndexPicos(double* histograma, int n, int* npicos);
float Media(double* hist, int n);
char IdentificaDigito(IVC* image);

//Histograma horizontal de uma imagem binario
int* BinHHist(IVC* image)
{
	int* h = (int*)calloc(image->height, sizeof(int));
	int pos = 0;
	for (int y = 0; y < image->height; y++)
	{

		for (int x = 0; x < image->width; x++)
		{
			pos = y * image->bytesperline + x;

			if (image->data[pos] == (unsigned char)255)h[y] = h[y] + 1;
		}

	}

	return h;
}

//Histograma horizontal de uma imagem binario
int* BinVHist(IVC* image)
{
	int* v = (int*)calloc(image->width, sizeof(int));
	int pos = 0;
	for (int x = 0; x < image->width; x++)
	{

		for (int y = 0; y < image->height; y++)
		{
			pos = y * image->bytesperline + x;

			if (image->data[pos] == 255)v[x] = v[x] + 1;
		}

	}

	return v;
}

void vc_timer(void) {
	static bool running = false;
	static std::chrono::steady_clock::time_point previousTime = std::chrono::steady_clock::now();

	if (!running) {
		running = true;
	}
	else {
		std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
		std::chrono::steady_clock::duration elapsedTime = currentTime - previousTime;

		// Tempo em segundos.
		std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(elapsedTime);
		double nseconds = time_span.count();

		std::cout << "Tempo decorrido: " << nseconds << "segundos" << std::endl;
		std::cout << "Pressione qualquer tecla para continuar...\n";
		std::cin.get();
	}
}




int main(void) {
	// V�deo
	char videofile[37] = "VC-TP2 - Enunciado\\Videos\\video1.mp4";
	cv::VideoCapture capture;
	struct
	{
		int width, height;
		int ntotalframes;
		int fps;
		int nframe;
	} video;
	// Outros
	std::string str;
	int key = 0;

	/* Leitura de v�deo de um ficheiro */
	/* NOTA IMPORTANTE:
	O ficheiro video.avi dever� estar localizado no mesmo direct�rio que o ficheiro de c�digo fonte.
	*/
	capture.open(videofile);
	
	/* Em alternativa, abrir captura de v�deo pela Webcam #0 */
	//capture.open(0, cv::CAP_DSHOW); // Pode-se utilizar apenas capture.open(0);
	
	/* Verifica se foi poss�vel abrir o ficheiro de v�deo */
	if (!capture.isOpened())
	{
		std::cerr << "Erro ao abrir o ficheiro de v�deo!\n";
		return 1;
	}



	/* N�mero total de frames no v�deo */
	video.ntotalframes = (int)capture.get(cv::CAP_PROP_FRAME_COUNT);
	/* Frame rate do v�deo */
	video.fps = (int)capture.get(cv::CAP_PROP_FPS);
	/* Resolu��o do v�deo */
	video.width = (int)capture.get(cv::CAP_PROP_FRAME_WIDTH);
	video.height = (int)capture.get(cv::CAP_PROP_FRAME_HEIGHT);

	/* Cria uma janela para exibir o v�deo */
	cv::namedWindow("VC - VIDEO", cv::WINDOW_AUTOSIZE);

	/* Inicia o timer */
	vc_timer();
	

	cv::Mat frame;
	while (key != 'q') {
		/* Leitura de uma frame do v�deo */
		capture.read(frame);

		/* Verifica se conseguiu ler a frame */
		if (frame.empty()) break;

		/* N�mero da frame a processar */
		video.nframe = (int)capture.get(cv::CAP_PROP_POS_FRAMES);

		/* Exemplo de inser��o texto na frame */
		str = std::string("RESOLUCAO: ").append(std::to_string(video.width)).append("x").append(std::to_string(video.height));
		cv::putText(frame, str, cv::Point(20, 25), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0), 2);
		cv::putText(frame, str, cv::Point(20, 25), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 1);
		str = std::string("TOTAL DE FRAMES: ").append(std::to_string(video.ntotalframes));
		cv::putText(frame, str, cv::Point(20, 50), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0), 2);
		cv::putText(frame, str, cv::Point(20, 50), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 1);
		str = std::string("FRAME RATE: ").append(std::to_string(video.fps));
		cv::putText(frame, str, cv::Point(20, 75), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0), 2);
		cv::putText(frame, str, cv::Point(20, 75), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 1);
		str = std::string("N. DA FRAME: ").append(std::to_string(video.nframe));
		cv::putText(frame, str, cv::Point(20, 100), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0), 2);
		cv::putText(frame, str, cv::Point(20, 100), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 1);

		// Fa�a o seu c�digo aqui...
		// Cria uma nova imagem IVC
		IVC* image = vc_image_new(video.width, video.height, 3, 255);

		// Copia dados de imagem da estrutura cv::Mat para uma estrutura IVC
		memcpy(image->data, frame.data, video.width * video.height * 3);
		// Executa uma fun��o da nossa biblioteca vc
		//vc_rgb_get_green(image);
		//BoundingBox(image);
		char *matricula = (char*)calloc(6,sizeof(char));
		try {
			Frame(image, matricula);
		}
		catch(std::exception e)
		{ }
		// Copia dados de imagem da estrutura IVC para uma estrutura cv::Mat
		memcpy(frame.data, image->data, video.width * video.height * 3);
		// Liberta a mem�ria da imagem IVC que havia sido criada
		vc_image_free(image);
		// +++++++++++++++++++++++++

		//Escrita da matricula no frame
		std::string m;
		std::stringstream ss;
		ss << matricula;
		ss >> m;
		cv::putText(frame, m, cv::Point(20, 125), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0), 2);
		cv::putText(frame, m, cv::Point(20, 125), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 1);

		/* Exibe a frame */
		cv::imshow("VC - VIDEO", frame);

		/* Sai da aplica��o, se o utilizador premir a tecla 'q' */
		if (cv::waitKey(1) == 'q')
			while (cv::waitKey(1) != 'q');
	}


	/* Para o timer e exibe o tempo decorrido */
	vc_timer();
	
	/* Fecha a janela */
	cv::destroyWindow("VC - VIDEO");

	/* Fecha o ficheiro de v�deo */
	capture.release();

	return 0;
}

//Fun�ao para a analise de um frame e desenho das caixas delimitadoras da matricula e das letras
void Frame(IVC* frame, char* c)
{
	IVC* frameg = vc_image_new(frame->width, frame->height, 1, 255);//imagem para o frame
	vc_rgb_to_gray(frame, frameg);//convers�o de rgb para bin�rio
	vc_write_image((char*)"frameg.pgm", frameg);

	int *h;
	int pos = 0;
	int v;
	int lmc = 20;//largura minima do caracter
	int amc = 50;//altura minima do caracter
	int width = frame->width;//largura do frame
	int ymin = 0;
	int ymax = 0;
	//Ciclo para percorrer o frame linha a linha com incrementos de amc
	for (int y = 0; y < frameg->height; y = y + amc)
	{

		h = GetRowProf(frameg, y);//histograma em escala de cinzento de uma determinada linha	
		int v = vhistogram(h, width, lmc, 100);//Divide o histograma em partes iguas e faz a diferen�a de contrates dentro delas e conta as
			//diferen��s que s�o superiores a 100
		if (v >= 6 && v <= 13)//se houver entre 6 a 13 essa linha interceta a matricula
		{
			//for (int x = 0; x < frame->width; x++)
			//{
			//	pos = y * frame->bytesperline + x * frame->channels;
			//	frame->data[pos] = (unsigned char)255;
			//	frame->data[pos + 1] = (unsigned char)255;
			//	frame->data[pos + 2] = (unsigned char)255;

			//}

			int* yh;
			int yv = 0;
			int i = y - amc;
			//ciclo para encontrar o extremo superior da matricula
			do
			{
				yh = GetRowProf(frameg,i);
				yv = vhistogram(yh, width, lmc, 100);

				if (yv >= 6 && yv <= 13)
				{
					if (i < y && i >= 10)
					{
						ymin = i - 10;
						free(yh);
						break;
					}
				}
				i++;
				free(yh);
			} while (i < y);

			
			//ciclo para encontrar o extremo inferior da matricula
			i = y + amc;
			do
			{
				yh = GetRowProf(frameg, i);
				yv = vhistogram(yh, width, lmc, 100);

				if (yv >= 6 && yv <= 13)
				{
					if (i > y && i >= 0)
					{
						ymax = i + 10;
						free(yh);
						break;
					}
				}
				free(yh);
				i--;
			} while (i > y);

			//for (int x = 0; x < frame->width; x++)
			//{
			//	pos = y * frame->bytesperline + x * frame->channels;
			//	frame->data[pos] = (unsigned char)255;
			//	frame->data[pos + 1] = (unsigned char)255;
			//	frame->data[pos + 2] = (unsigned char)255;
			//}
		}
		free(h);		
	}

	int height = ymax - ymin;//Calcular a altura da matricula
	int xmin = 0, xmax = 0;
	
	if (height > 10 && height < 200)
	{
		IVC* row = vc_image_new(frameg->width, height, 1, 255);//nova imagem para essa linha toda aonde est� a matricula

		//Copiar essa linha da imagem original em escala de cinzento para a imagem row
		int posk = 0, yk = 0;
		for (int y = ymin; y < ymax; y++)
		{
			for (int x = 0; x < row->width; x++)
			{
				pos = y * row->width + x;
				posk = yk * row->width + x;
				if (posk >= 0 && pos >= 0 && posk <= row->width * row->height && pos <= frameg->width * frameg->height)
				{
					row->data[posk] = frameg->data[pos];
				}
			}
			yk++;
		}
		int b = 0;
		//vc_write_image((char*)"row.pgm", row);
		IVC* rb = vc_image_new(row->width, row->height, 1, 255);
		vc_gray_to_binary_bernsen(row, rb, 5, 230);//Convers�o de escala de cinzentos para bin�rio
		//vc_write_image((char *)"rowb.pgm", rb);
		IVC *rc = vc_image_new(row->width, row->height, 1, 255);
		vc_binary_open(rb, rc, 5);//Opera��o morfol�gica de abertura
		//vc_write_image((char*)"rowc.pgm", rc);

		int* hv = BinVHist(rc);//Histograma vertical
		int* hh = BinHHist(rc);//Histograma horozontal
	
		//Encontra o aonde a matricula come�a no eixo horizontal
		for (int i = (int)rc->width/2; i > 0; i--)
		{
			if (hv[i] == 0) {
				xmin = i - 1;
				break;
			}
		}
		//Encontra o aonde a matricula acaba no eixo horizontal
		for (int i = (int)rc->width / 2; i < rc->width; i++)
		{
			if (hv[i] == 0) {
				xmax = i + 1;
				break;
			}
		}

		free(hv);
		free(hh);
		
		//Desenho da caixa � volta da matricula
		for (int x = xmin; x < xmax; x++)
		{
			pos = ymin * frame->bytesperline + x * frame->channels;
			if (pos > 0 && pos < frame->height * frame->bytesperline)
			{
				frame->data[pos] = (unsigned char)0;
				frame->data[pos + 1] = (unsigned char)255;
				frame->data[pos + 2] = (unsigned char)0;
			}
		}

		for (int x = xmin; x < xmax; x++)
		{
			pos = ymax * frame->bytesperline + x * frame->channels;
			if (pos > 0 && pos < frame->height * frame->bytesperline)
			{
				frame->data[pos] = (unsigned char)0;
				frame->data[pos + 1] = (unsigned char)255;
				frame->data[pos + 2] = (unsigned char)0;
			}
		}

		for (int y = ymin; y < ymax; y++)
		{
			pos = y * frame->bytesperline + xmin * frame->channels;
			if (pos > 0 && pos < frame->height * frame->bytesperline)
			{
				frame->data[pos] = (unsigned char)0;
				frame->data[pos + 1] = (unsigned char)255;
				frame->data[pos + 2] = (unsigned char)0;
			}
		}

		for (int y = ymin; y < ymax; y++)
		{
			pos = y * frame->bytesperline + xmax * frame->channels;
			if (pos > 0 && pos < frame->height * frame->bytesperline)
			{
				frame->data[pos] = (unsigned char)0;
				frame->data[pos + 1] = (unsigned char)255;
				frame->data[pos + 2] = (unsigned char)0;
			}
		}
		
		int width = xmax - xmin;//Largura da matricula
		IVC* matricula = vc_image_new(width, height, 1, 255);//Imagem para a matricula
		yk = 0;
		pos = 0;
		posk = 0;
		int xk = 0;
		//Copia a matricula para a imagem criada
		for (int y = ymin; y < ymax; y++)
		{
			for (int x = xmin; x < xmax; x++)
			{
				pos = y * frameg->width + x;
				posk = yk * width + xk;
				if (posk > 0 && posk < matricula->width * matricula->height && pos > 0 && pos < frameg->width * frameg->height);
				{
					matricula->data[posk] = frameg->data[pos];
				}
				xk++;
			}
			yk++;
			xk = 0;
		}
		
		if (matricula != NULL)
		{
			//vc_write_image((char*)"matricula.pgm", matricula);
			IVC* mb = vc_image_new(matricula->width, matricula->height, 1, 255);
			//vc_gray_edge_prewitt(matricula, mb, 0.8);
			vc_gray_to_binary_bernsen(matricula, mb, 5, 150);//Convers�o de escala de cinzentos para bin�rio
			//vc_write_image((char*)"mb.pgm", mb);		
			IVC* mc = vc_image_new(matricula->width, matricula->height, 1, 255);
			vc_binary_open(mb, mc, 3);//Opera��o morfol�gica de abertura
			//vc_write_image((char*)"mo.pgm", mc);
			vc_bin_negative(mc);//Negativo da matricula
			vc_write_image((char*)"mn.pgm", mc);

			int nletras = 0;
			OVC* letras = vc_binary_blob_labelling(mc, matricula, &nletras);//Etiquetagem da imagem da matricula
			vc_binary_blob_info(matricula, letras, nletras);//Preenchimento do array com informa��o das etiquetas da imagen

			//vc_write_image((char*)"ml.pgm", matricula);

			IVC* caracter[6];//array de imagens para os caracteres da matricula
			for (int i = 0; i < 6; i++)caracter[i] = NULL;//Inicializar tudo a nulo

			float r = 0;
			int k = 0;
			int areatotal = matricula->height * matricula->width;
			//percorrer o array de blobs e encontrar as letras
			for (int j = 0; j < nletras; j++)
			{
				r = (float)letras[j].width / (float)letras[j].height;
				if (r >= 0.4 && r < 1 && letras[j].area > areatotal * 0.0065)//Condi��es que determinam o quq � um caracter
				{
					//desenho das caixas delimitadoras � volta da letras
					for (int x = xmin + letras[j].x; x < xmin + letras[j].x + letras[j].width; x++)
					{
						pos = (ymin + letras[j].y) * frame->bytesperline + x * frame->channels;
						if (pos > 0 && pos < frame->height * frame->bytesperline)
						{
							frame->data[pos] = (unsigned char)0;
							frame->data[pos + 1] = (unsigned char)255;
							frame->data[pos + 2] = (unsigned char)0;
						}
					}

					for (int x = xmin + letras[j].x; x < xmin + letras[j].x + letras[j].width; x++)
					{
						pos = (ymin + letras[j].y + letras[j].height) * frame->bytesperline + x * frame->channels;
						if (pos > 0 && pos < frame->height * frame->bytesperline)
						{
							frame->data[pos] = (unsigned char)0;
							frame->data[pos + 1] = (unsigned char)255;
							frame->data[pos + 2] = (unsigned char)0;
						}
					}

					for (int y = ymin + letras[j].y; y < ymin + letras[j].y + letras[j].height; y++)
					{
						pos = y * frame->bytesperline + (xmin + letras[j].x) * frame->channels;
						if (pos > 0 && pos < frame->height * frame->bytesperline)
						{
							frame->data[pos] = (unsigned char)0;
							frame->data[pos + 1] = (unsigned char)255;
							frame->data[pos + 2] = (unsigned char)0;
						}
					}

					for (int y = ymin + letras[j].y; y < ymin + letras[j].y + letras[j].height; y++)
					{
						pos = y * frame->bytesperline + (xmin + letras[j].x + letras[j].width) * frame->channels;
						if (pos > 0 && pos < frame->height * frame->bytesperline)
						{
							frame->data[pos] = (unsigned char)0;
							frame->data[pos + 1] = (unsigned char)255;
							frame->data[pos + 2] = (unsigned char)0;
						}
					}

					//Criar uma imagem nova no array para o caracter encontrado 
					caracter[k] = vc_image_new(letras[j].width + 2, letras[j].height + 2, 1, 255);//Uma imagem nova para cada caracter
					posk = 0;
					pos = 0;
					xk = 0;
					yk = 0;
					//copiar o caracter para essa imagem
					for (int y = letras[j].y - 1; y < letras[j].y + letras[j].height + 1; y++)
					{
						for (int x = letras[j].x - 1; x < letras[j].x + letras[j].width + 1; x++)
						{
							pos = y * mc->width + x;
							posk = yk * (letras[j].width + 2) + xk;
							caracter[k]->data[posk] = mc->data[pos];
							xk++;
						}
						yk++;
						xk = 0;
					}
					
					//vc_write_image((char*)"caracter.pgm" , caracter[k]);
					k++;
				}
			}

			//identifica��o dos 6 caracteres
			for (int i = 0; i < 6; i++)
			{
				vc_write_image((char*)"caracter.pgm", caracter[i]);
				c[i] = IdentificaCaracter(caracter[i]);
			}

			//for (int i = 0; i < 6; i++)
			//{
			//	printf("%c\n", c[i]);
			//}

			free(c);
			free(letras);
			free(mb);
			free(mc);
		}		
		free(matricula);		
		free(rc);
		free(rb);
		free(row);		
	}
	free(frameg);
}

//Histograma horizontal de uma imagem binaria
double* HistogramaHorizontalF(IVC* image)
{
	double* h = (double*)calloc(image->height, sizeof(double));
	int pos = 0;
	for (int y = 0; y < image->height; y++)
	{

		for (int x = 0; x < image->width; x++)
		{
			pos = y * image->bytesperline + x;

			if (image->data[pos] == (unsigned char)255)h[y] = h[y] + 1;
		}
	}
	return h;
}

//Histograma vertical de uma imagem binaria
double* HistogramaVerticalF(IVC* image)
{
	double* v = (double*)calloc(image->width, sizeof(double));
	int pos = 0;
	for (int x = 0; x < image->width; x++)
	{
		for (int y = 0; y < image->height; y++)
		{
			pos = y * image->width + x;

			if (image->data[pos] == (unsigned char)255)v[x] = v[x] + 1;
		}
	}
	return v;
}

//Fun��o para identificar um caracter apartir de uma imagem
char IdentificaCaracter(IVC* image)
{
	if (image != NULL)
	{
		IVC* open = vc_image_new(image->width, image->height, 1, 255);
		vc_binary_open(image, open, 3);
		vc_write_image((char*)"caractero.pgm", open);
		//Histograma vertical e horizontal da imagem
		double* histVertical = HistogramaVerticalF(open);
		double* histHorizontal = HistogramaHorizontalF(open);
		char caracter;
		//Aplicar a media movel aos histogramas
		mediaMovel(histHorizontal, image->height);
		mediaMovel(histVertical, image->width);
		//Aplicar a normaliza��o aos histogramas
		normalizaHist(histHorizontal, image->height, image->width);
		normalizaHist(histVertical, image->width, image->height);
		int npicosH = 0;//numero de picos no histograma horizontal
		int npicosV = 0;//numero de picos no histograma vertical
		int* picosH = IndexPicos(histHorizontal, image->height, &npicosH);//array com o os indices dos picos horizontais
		int* picosV = IndexPicos(histVertical, image->width, &npicosV);//array com o os indices dos picos verticais

		printf("%d\n", npicosH);
		printf("%d\n", npicosV);

		for (int i = 0; i < npicosH; i++)printf("Index: %d --> %f\n", picosH[i], histHorizontal[picosH[i]]);
		for (int i = 0; i < npicosV; i++)printf("Index: %d --> %f\n", picosV[i], histVertical[picosV[i]]);


		//Reconehcimento da letra recebida atrav�s do numero de picos no histograma horizontal e vertical e pela posi��o relativa desses picos na imagem
		if (npicosH == 1 && (npicosV == 1 || npicosV == 2) && picosH[0] < image->height / 3)return '7';
		if (npicosH == 3 && (npicosV == 3 || npicosV == 2) && picosH[0] < image->height / 3 && picosH[2] > image->height * 2 / 3 && picosV[0] < image->width / 3 && picosV[1] > image->width * 2/3 && picosH[2] - picosH[1] < 10)return 'Q';
		if ((npicosH == 1 || npicosH == 2) && npicosV == 2 && picosV[0] < image->width / 3 && image->width * (2 / 3) < picosV[1] && (picosH[0] > image->height * 2 / 3 || picosH[1] > image->height * 2 / 3))return 'U';
		if (npicosH == 2 && (npicosV == 2 || npicosV == 3) && picosV[0] < image->width / 3)return 'R';
		if (npicosH == 3 && npicosV == 2 && picosV[1] > image->width * 2 / 3 && histVertical[picosV[1]] > histVertical[picosV[0]] && picosH[2] - picosH[1] > 10)return '9';
		if ((npicosH == 3 || npicosH == 4) && (npicosV == 3 || npicosV == 2) && picosV[0] < image->width / 3 && histVertical[picosV[0]] > histVertical[picosV[1]] && histHorizontal[picosH[1]]> histHorizontal[picosH[0]])return '6';
		if ((npicosH == 4 || npicosH == 3 || npicosH == 5 || npicosH == 6) && npicosV == 2 && picosH[0] < image->height / 3 && picosV[0] < image->width / 3 && picosV[1] > image->width * 2/3 && picosH[1] < image->height * 2 / 3 && picosH[1] > image->height / 3)return '8';
		if (npicosH == 2 && (npicosV == 1 || npicosV == 3) && picosH[1] > image->height * 2 / 3 && picosV[0] > image->width / 3 && picosV[0] < image->width * 2 / 3)return '2';
		


		free(open);
		free(histHorizontal);                      
		free(histVertical);
		free(picosH);
		free(picosV);                                                                                                                                      
	}
}

//Faz a media de um histograma
float Media(double* hist, int n)
{
	int soma = 0;
	for (int i = 0; i < n; i++)
	{
		soma = soma + hist[i];
	}
	return soma / n;
}

//Fun��o para encontrar o indice dos picos de um histograma
int* IndexPicos(double* histograma, int n, int *npicos)
{
	int picos[30];
	for (int i = 0; i < 30; i++)picos[i] = 0;
	int j = 0;
	int k = 0;
	while(j < n - 1)
	{
		if (histograma[j] < histograma[j + 1])
		{
			while (histograma[j] < histograma[j + 1])
			{
				j++;
			}
			if (histograma[j] >= 0.4)
			{
				picos[k] = j;
				k++;
			}
		}
		else
		{
			j++;
		}
	}

	int* p = (int*)calloc(k, sizeof(int));
	for (int i = 0; i < k; i++)
	{
		p[i] = picos[i];
	}
	*npicos = k;
	return p;
}

//Normaliza o histograma
void normalizaHist(double* histograma, int n, int totalPixeis)
{
	if (totalPixeis > 0)
	{
		for (int i = 0; i < n; i++)
		{
			histograma[i] = (histograma[i] / totalPixeis);
		}
	}
}

//Aplica a m�dia movel a um histograma
void mediaMovel(double* histograma, int n)
{
	float soma = 0;
	for (int i = 0; i < n; i++)
	{
		for (int j = i - 2; j <= i + 2; j++)
		{
			if(j < n && j >= 0) soma = soma + histograma[j];
		}
		histograma[i] = soma / 5;
		soma = 0;
	}
}

//Histograma horizontal de uma imagem em escala de cinzentos
int* GetColunmProf(IVC* image)
{
	int x, y, soma, pos;
	int* h = (int *)malloc(sizeof(int) * image->width);

	for (x = 0; x < image->width; x++)
	{
		soma = 0;

		for (y = 0; y < image->height; y++)
		{
			pos = y * image->bytesperline + x;

			if (image->data[pos] == 0)soma++;
		}
		h[x] = soma;
	}
	return h;
}

//Histograma vertical de uma imagem em escala de cinzentos
int *GetRowProf(IVC* image, int y)
{
	int pos = 0;
	int *h = (int*)malloc(sizeof(int) * image->width);
	
	for (int x = 0; x < image->width; x++)
	{
		pos = y * image->bytesperline + x * image->channels;
		if (pos > 0 && pos < image->height * image->width)
		{
			h[x] = (int)image->data[pos];
			
		}
	}
	return h;
}

//Fun��o para encontrar o numero de zonas de contraste de um histograma dividindo-o em v�rias partes iguais, encontrado o maximo e minimo em cada
//uma dessas partes e fazendo a diferen�a entre eles.
int vhistogram(int h[], int n, int kernel, int cmin)
{
	int max, min;
	int v = 0;
	for (int i = 0; i < n; i = i + kernel)
	{
		max = 0;
		min = 255;
		int c = i;
		max = h[c];
		for (c = i + 1; c < i + kernel; c++) {
			if (h[c] > max) {
				max = h[c];
			}
		}
		c = i;
		min = h[c];
		for (c = i + 1; c < i + kernel; c++) {
			if (h[c] < min) {
				min = h[c];
			}
		}

		if (max - min > cmin)v++;
	}
	return v;
}


int BoundingBox(IVC* src)
{
	int pos = 0;
	IVC* gray = vc_image_new(src->width, src->height, 1, 255);//Imagem em cinzento para converter a imagem para cinzento
	IVC* bin = vc_image_new(src->width, src->height, 1, 255);//Imagen em binario para converter a imagem para binario
	IVC* close = vc_image_new(src->width, src->height, 1, 255);//Imagem em bin para fazer um fecho close 
	IVC* labeled = vc_image_new(src->width, src->height, 1, 255);//Imagem para os labels

	//A imagem � convertida para escala de cinzentos e depois para bin�rio para ser mais de trabalhar com a imagem
	vc_rgb_to_gray(src, gray);//Converter a imagem original para Escala de cinzentos
	//vc_write_image("gray.pgm", gray);
	vc_gray_to_binary_bernsen(gray, bin, 5, 100);//Converter para bin�rio usando o metodo de bernsen com um cmin de 240 para encontrar zonas de alto											 //contraste na imagem
	vc_binary_close(bin, close, 5);//Fazer um fecho da imagem com kernerl de 5
	//vc_write_image("bin.pgm", bin);
	//vc_write_image("close.pgm", close);	
	int labels = 0;//Vari�vel para o n�mero de labels encontrados
	OVC* blobs = vc_binary_blob_labelling(close, labeled, &labels);//Fazer o labelling da imagem para uma vari�vel que vai conter a informa��o de todos																//os blobs na imagem, 1 deles sendo a matricula
	//vc_write_image("labeled.pgm", labeled);
	//printf("Labels: %d\n", labels);
	vc_binary_blob_info(labeled, blobs, labels);//Preencher o array de blobs com a sua informa��o(�rea,perimetro,largura,altura,Xinicial,Yinicial)
	//Filtragem dos blobls encontrados
	for (int i = 0; i < labels; i++)//Percorrer o array de blobs
	{
		if (blobs[i].height != 0 && blobs[i].width != 0 && blobs[i].area != 0)//Filtrar os blobs com valores inv�lidos
		{
			float r = blobs[i].width / blobs[i].height;//Calculo do racio largura/altura
			if (r >= 4 && r <= 5 && blobs[i].area > 2300 && blobs[i].area < 25000 && blobs[i].x != 1)//Se o racio estiver dentro dde 4 e 5 e a area for maior que 5000
			{
				
				printf("_______________BLOB________________\n");
				printf("Label: %d\n", blobs[i].label);
				printf("Width: %d\n", blobs[i].width);
				printf("Height: %d\n", blobs[i].height);
				printf("Area: %d\n", blobs[i].area);
				printf("X: %d\n", blobs[i].x);
				printf("Y: %d\n", blobs[i].y);

				//Criar uma imagem nova do tamanho do blob e copia-lo para l�
				IVC* blob = vc_image_new(blobs[i].width, blobs[i].height, 1, 255);//Imagem nova para analisar o blob e determinar se pode ser
																				//uma matricula ou n�o, com as dimens�es do blob
				int bwidth = blobs[i].width;
				int bheight = blobs[i].height;

				int pos = 0;//Posi��o na imagem original
				int posb = 0;//Posi��o na imagem para o blob
				int xb = 0, yb = 0;
				//Copiar o blob encontrado, da imagem original para um imagem nova para ser analisado
				for (int y = blobs[i].y; y < bheight + blobs[i].y; y++)
				{
					for (int x = blobs[i].x; x < bwidth + blobs[i].x; x++)
					{
						pos = y * close->width + x;//Posi��o na imagem original
						posb = yb * bwidth + xb;//Posi��o na imagem do blob
						blob->data[posb] = gray->data[pos];//Copiar os dados
						xb++;//Incrementar uma coluna � posi��o no blob
					}
					yb++;//Incrementar uma linha � posi��o no blob
					xb = 0;//Por a posi��o nas colunas do blob a zero
				}

				//Desenho do limite superior do blob 
				for (int x = blobs[i].x; x < blobs[i].x + blobs[i].width; x++)
				{
					pos = blobs[i].y * src->bytesperline + x * src->channels;//Calculo da posi�ao
					src->data[pos] = 0;//R
					src->data[pos + 1] = 255;//G
					src->data[pos + 2] = 255;//B
				}
				//Desenho do limite inferior do blob 
				for (int x = blobs[i].x; x < blobs[i].x + blobs[i].width; x++)
				{
					pos = (blobs[i].y + blobs[i].height) * src->bytesperline + x * src->channels;//Calculo da posi�ao
					src->data[pos] = 0;//R
					src->data[pos + 1] = 255;//G
					src->data[pos + 2] = 255;//B
				}
				//Desenho do limite esquerdo do blob 
				for (int y = blobs[i].y; y < blobs[i].y + blobs[i].height; y++)
				{
					pos = y * src->bytesperline + blobs[i].x * src->channels;//Calculo da posi�ao
					src->data[pos] = 0;//R
					src->data[pos + 1] = 255;//G
					src->data[pos + 2] = 255;//B
				}
				//Desenho do limite direito do blob 
				for (int y = blobs[i].y; y < blobs[i].y + blobs[i].height; y++)
				{
					pos = y * src->bytesperline + (blobs[i].x + blobs[i].width) * src->channels;//Calculo da posi�ao
					src->data[pos] = 0;//R
					src->data[pos + 1] = 255;//G
					src->data[pos + 2] = 255;//B
				}
			#pragma region MyRegion

				//vc_write_image("blob.pgm", blob);

				IVC* matriculabin = vc_image_new(blob->width, blob->height, 1, 255);//Imagem para a imagem binaria do blob
				//IVC* matriculaclose = vc_image_new(blob->width, blob->height, 1, 255);//Imagem para o fecho
				IVC* matriculalabel = vc_image_new(blob->width, blob->height, 1, 255);//Imagem para os labels

				vc_gray_to_binary_bernsen(blob, matriculabin, 5, 240);//Convers�o para binario com o metodo de bernsen
				//vc_write_image("matriculabin.pgm", matriculabin);
				vc_binary_close(matriculabin, blob, 5);//Opera��o morfol�gica de fecho com kerner 3x3				
				//vc_write_image("matriculaclose.pgm", matriculaclose);
				vc_bin_negative(blob);//Negativo do close
				int nletras = 0;//Vari�vel para o numero de blobs
				//vc_write_image("matriculaclose.pgm", matriculaclose);
				OVC* letras = vc_binary_blob_labelling(blob, matriculalabel, &nletras);//Fazer o labelling da imagem
				//vc_write_image("matriculalabel.pgm", matriculalabel);

				vc_binary_blob_info(matriculalabel, letras, nletras);//Preencher o array com a informa��o de cada label
				int w = 0;
				//Verificar quais sao os blobs validos
				if (nletras >= 6)//Se o numero de blobs encontrados for menor 6 � descartado
				{

					for (int l = 0; l < nletras; l++)//Percorrer os array de labels
					{
						r = (float)letras[l].width / (float)letras[l].height;//Racio de largura/altura
						if (r > 0.25 && r < 0.9 && letras[l].area > 130)
						{
							w++;
							/*
							O desenho das letras � o mesmo processo da matricula sendo simplesmente com as dimens�es das letras
							*/
							//Desenho do limite superior do blob 
							for (int x = letras[l].x + blobs[i].x; x < letras[l].x + letras[l].width + blobs[i].x; x++)
							{
								pos = (letras[l].y + blobs[i].y) * src->bytesperline + x * src->channels;//Calculo da posi�ao
								src->data[pos] = 255;//R
								src->data[pos + 1] = 0;//G
								src->data[pos + 2] = 0;//B
							}
							//Desenho do limite inferior do blob 
							for (int x = letras[l].x + blobs[i].x; x < letras[l].x + letras[l].width + blobs[i].x; x++)
							{
								pos = (letras[l].y + letras[l].height + blobs[i].y) * src->bytesperline + x * src->channels;//Calculo da posi�ao
								src->data[pos] = 255;//R
								src->data[pos + 1] = 0;//G
								src->data[pos + 2] = 0;//B
							}
							//Desenho do limite esquerdo do blob 
							for (int y = letras[l].y + blobs[i].y; y < letras[l].y + letras[l].height + blobs[i].y; y++)
							{
								pos = y * src->bytesperline + (blobs[i].x + letras[l].x) * src->channels;//Calculo da posi�ao
								src->data[pos] = 255;//R
								src->data[pos + 1] = 0;//G
								src->data[pos + 2] = 0;//B
							}
							//Desenho do limite direito do blob 
							for (int y = letras[l].y + blobs[i].y; y < letras[l].y + letras[l].height + blobs[i].y; y++)
							{
								pos = y * src->bytesperline + (letras[l].x + letras[l].width + blobs[i].x) * src->channels;//Calculo da posi�ao
								src->data[pos] = 255;//R
								src->data[pos + 1] = 0;//G
								src->data[pos + 2] = 0;//B
							}
						}
					}

					if (w == 6)
					{
						/*
					O desenho da bounding box da matricula � feita por partes, fazendo primeiro o desenho da reta superior percorrendo toda a
					largura da matricula na mesma linha, depois a reta inferior da bounding box pelo mesmo processom. O desenho das laterais
					da bounding box � feito percorrendo toda a altura da matricula numa coluna fixa.
					*/
					//Desenho do limite superior do blob 
						for (int x = blobs[i].x; x < blobs[i].x + blobs[i].width; x++)
						{
							pos = blobs[i].y * src->bytesperline + x * src->channels;//Calculo da posi�ao
							src->data[pos] = 0;//R
							src->data[pos + 1] = 255;//G
							src->data[pos + 2] = 255;//B
						}
						//Desenho do limite inferior do blob 
						for (int x = blobs[i].x; x < blobs[i].x + blobs[i].width; x++)
						{
							pos = (blobs[i].y + blobs[i].height) * src->bytesperline + x * src->channels;//Calculo da posi�ao
							src->data[pos] = 0;//R
							src->data[pos + 1] = 255;//G
							src->data[pos + 2] = 255;//B
						}
						//Desenho do limite esquerdo do blob 
						for (int y = blobs[i].y; y < blobs[i].y + blobs[i].height; y++)
						{
							pos = y * src->bytesperline + blobs[i].x * src->channels;//Calculo da posi�ao
							src->data[pos] = 0;//R
							src->data[pos + 1] = 255;//G
							src->data[pos + 2] = 255;//B
						}
						//Desenho do limite direito do blob 
						for (int y = blobs[i].y; y < blobs[i].y + blobs[i].height; y++)
						{
							pos = y * src->bytesperline + (blobs[i].x + blobs[i].width) * src->channels;//Calculo da posi�ao
							src->data[pos] = 0;//R
							src->data[pos + 1] = 255;//G
							src->data[pos + 2] = 255;//B
						}
					}
					w = 0;
				}
				free(letras);//Libertar a memoria usada para a estrutura que guarda a informa��o das letras
#pragma endregion
			}
		}
	}
	free(blobs);//Libertar a memoria usada para a estrutura que guarda a informa��o das matriculas
	free(bin);
	free(gray);
	free(labeled);
	return 1;
}
