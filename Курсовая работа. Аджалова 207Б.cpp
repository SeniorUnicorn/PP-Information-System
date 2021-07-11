/*************************************************************************
*                                                                        *
*               Программирование на языке высокого уровня                *
*                                                                        *
**************************************************************************
*                                                                        *
*      Project type:  Win32 Console Application                          *
*      Project name:  Курсовая работа                                    *
*      File name   :  Курсовая работа. Аджалова 207Б.cpp                 *
*      Language    :  cpp MSVS 2019                                      *
*      Programmers :  M30-207Б-19                                        *
*                     Аджалова Севиль Фархадовна                         *
*      Modified by :  03.12.2020                                         *
*      Created     :  05.09.2020                                         *
*      Comment     :  Система поддержки продаж магазина                  *
*                                                                        *
*************************** З А Д А Н И Е ********************************
*                                                                        *
*   1.Сформулировать функциональные и нефункциональные требования к      *
*     информационной файловой системе своего варианта.                   *
*   2.Спроектировать структуры данных для заданной предметной области.   *
*   3.Реализовать информационную систему средствами С++ используя        *
*     бинарные файлы и структуры.                                        *
*                                                                        *
*   Предметная область - специализированный магазин по продаже           *
*   аудиовидеотехники.                                                   *
*   Решаемые задачи:                                                     *
*    •  учет поставщиков;                                                *
*    •  учет поступления товаров на склад;                               *
*    •  учет реализации товаров;                                         *
*    •  формирование счетов на оплату товаров.                           *
*                                                                        *
********** Ф А Й Л Ы   В   Д И Р Е К Т О Р И И   П Р О Е К Т А ***********
*                                                                        *
*   Резервные копии корректных версий файлов баз данных:                 *
*    •  resMoves.bin - рез. копия таблицы движений                       *
*    •  resPost.bin  - рез. копия списка поставщиков                     *
*    •  resTov.bin   - рез копия списка товаров                          *
*   Базы данных                                                          *
*     см. "глобальные константы"                                         *
*   Счета и отчеты                                                       *
*     см. "глобальные константы"                                         *
*   Текстовые файлы                                                      *
*    •  contractors.txt - список поставщиков                             *
*    •  store.txt       - список товаров                                 *
*    •  structure.txt   - структура файлов, используемых в программе     *
*                                                                        *
*************************************************************************/

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
using namespace std;

/*************************************************************************
 *               Г Л О Б А Л Ь Н Ы Е    К О Н С Т А Н Т Ы                *
 ************************************************************************/

//файлы в директории проекта
//исходные файлы баз данных
const char* CONTRACTORS = "contractors.bin";	//база поставщиков
const char* STORE = "store.bin";				//база товаров в наличии
const char* TRAFFIC = "traffic.bin";			//таблица движений
//файлы с результатами работы программы
const char* PURCHASE_INVOICE = "buy_in.bin";	//счет на закупку
const char* SALE_INVOICE = "sale_in.bin";		//счет на продажу
const char* REPORT = "account.bin";				//отчет за период

//максимальная длина названия товара
const int MAX_TOV = 40;

//максимальная длина названия поставщика
const int MAX_POST = 30;

//максимальный ID товара/поставщика
const int MAX_ID = 9999;

//максимаьлная длина строки
const int LEN = 256;

/*************************************************************************
 *                  П Р О Т О Т И П Ы    Ф У Н К Ц И Й                   *
 ************************************************************************/

//основные процессы
int   purchase		(void);
int   sale			(void);
int   report		(void);
int   print			(const int& mode);

//обновление базы
int   addToBase		(const int& mode, struct part& product);
int   makeStock		(const int& mode, struct part& product);
int   finder		(const tm& begin, const tm& end, const int& mode, const int& ID);

//инструменты для получения информации от пользователя
int   getPost		(struct part& product);
int	  getTovP		(struct part& product);
int	  getTovS		(struct part& product);
int   getPeriod		(tm& begin, tm& end);
int   getMode		(int& code);

//инструменты для поиска данных
int   fillProduct	(const int& tovID, struct part& product);
int   findName		(const int& mode, const int& ID, char(*name));
int   checkExistence(const int& mode, const int& ID, const char(*name));
int   newID			(const int& mode);

//инструменты для работы с датами
tm	  convertDate	(const char(*word));
tm	  today			(void);
bool  cmpDate		(const tm& first, const tm& second);

//вспомогательные инструменты
char* eng			(const char(*rus), char(*res));
bool  checkString	(const char(*string));
int   getInt		(const char(*string));
int   cpyFile		(const int& mode, const char(*src_file), FILE* temp);
int   checkFile		(const int& mode, const char(*src_file));
int   cleanFile		(const char(*src_file));
int   pasteProduct	(FILE* base, const int& mode, struct part& product);
int   readProduct	(FILE* base, const int& mode, struct part& product, const long& place);

//инструменты для печати данных
int   printList		(const int& mode, const int& postID);
void  printMessage	(const int& error_code);
int   printFile		(const int& mode);
void  printHeader	(const int& mode);
void  printString	(const int& mode, struct part& product, const bool& flag);
void  printBorder	(const int& mode, const int& sum);

/*************************************************************************
 *                         С Т Р У К Т У Р Ы                             *
 ************************************************************************/

struct part {
	int  tovID = 0;								//ID товара
	int  postID = 0;							//ID поставщика
	char tovName[MAX_TOV + 1] = "";				//название товара
	char postName[MAX_POST + 1] = "";			//название поставщика
	int  count = 0;								//количество товара
	int  cost = 0;								//цена единицы товара
	int  sum = cost * count;					//стоимость
	tm   date = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };	//дата закупки/продажи
};//end of part

/*************************************************************************
 *                 О С Н О В Н А Я    П Р О Г Р А М М А                  *
 ************************************************************************/

int main() {
	int code;						//номер команды
	int res = 0;					//код результата работы программы
	char s[LEN + 1] = "";			//технологическая строка для печати

	system("color F0");				//экран белый, буквы черные
	system("cls");					//очистка экрана

	printf(eng("Добро пожаловать в автоматизированную систему учета", s));
	printf(eng(" продаж и поставок!\nДанная программа позволяет", s));
	printf(eng(" вести учет поставщиков, учет поступления\nтоваров", s));
	printf(eng(" на склад и их реализации а также предоставляет о", s));
	printf(eng("тчеты за \nопределнный период и счета для оплаты.\n", s));
	
	do {							//цикл до завершения работы программы
		//описание доступных команд
		printf(eng("Введите:\n1 - для внесения закупки в базу и ", s));
		printf(eng("формирования счета поставщику\n2 - для", s));
		printf(eng(" внесения продажи в базу и формимрования счета", s));
		printf(eng(" покупателю\n3 - для формирования отчета по ", s));
		printf(eng("закупкам/продажам за указанный период\n4 - для ", s));
		printf(eng("печати последнего счета на закупку/продажу или", s));
		printf(eng(" отчета за указанный период\n5 - для печати ", s));
		printf(eng("списка товаров на складе/списка поставщиков/", s));
		printf(eng("таблицы движений\n0 - для завершения работы", s));
		printf(eng(" программы\n\nВвод: ", s));
		
		//получение номера команды
		code = getInt(gets_s(s));
		while ((code == INT_MAX) || (code < 0) || (code > 5)) {
			printf(eng("\nВведена недопустимая команда.", s));
			printf(eng(" Повторите попытку.\n\nВвод: ", s));
			code = getInt(gets_s(s));
		}//end while

		switch (code) {				//вызов подпрограммы, реализующей:
		case 1:						//закупка
			printf(eng("\nВыбрано внесение закупки в базу.\n", s));
			res = purchase();
			printMessage(res);
			break;

		case 2:						//продажа
			printf(eng("\nВыбрано внесение продажи в базу.\n", s));
			res = sale();
			printMessage(res);
			break;

		case 3:						//создание отчета
			printf(eng("\nВыбрано формирование отчета.\n", s));
			res = report();
			printMessage(res);
			break;

		case 4:						//печать счетов и отчетов
			printf(eng("\nВыбрана печать последнего счета", s));
			printf(eng(" на закупку/продажу или отчета.\n", s));
			res = print(1);
			printMessage(res);
			break;

		case 5:						//печать баз данных
			printf(eng("\nВыбрана печать списка товаров", s));
			printf(eng(" на складе/списка поставщиков", s));
			printf(eng("/таблицы движений.\n", s));
			res = print(2);
			printMessage(res);
			break;

		case 0:						//завершение работы
			printf(eng("\n\nЗавершение работы...\n\n", s));
			break;
		}//end switch

		//произошла ошибка при работе с файлами
		if ((res != 0) && (res != -1) && (res != -6) && (res != -7)) {
			return res;				//вернуть код ошибки
		}//end if

	} while (code != 0);//end while

	return 0;						//завершение без ошибок

}//end main()

/*************************************************************************
 *                 Р Е А Л И З А Ц И Я    Ф У Н К Ц И Й                  *
 ************************************************************************/

 /*=======================================================================/
 / Основные процессы                                                      /
 /=======================================================================*/

//закупка товаров
int purchase(void) {
	int code;						//ID товара/поставщика
	int error;						//код ошибки
	FILE* tmpSTORE;					//резервная копия списка товаров
	FILE* tmpTRAFFIC;				//резервная копия таблицы движений
	FILE* tmpCONTRACTORS;			//резервная копия списка поставщиков
	struct part product;			//товар

	//создание резервной копии списка товаров
	tmpSTORE = tmpfile();
	if (!tmpSTORE) {				//файл не найден
		return -2;					//вернуть код ошибки
	}//end if
	error = cpyFile(10, STORE, tmpSTORE);
	if (error != 0) {				//произошла ошибка
		return error;				//вернуть код ошибки
	}//end if

	//создание резервной копии списка поставщиков
	tmpCONTRACTORS = tmpfile();
	if (!tmpCONTRACTORS) {			//файл не найден
		return -2;					//вернуть код ошибки
	}//end if
	error = cpyFile(12, CONTRACTORS, tmpCONTRACTORS);
	if (error != 0) {				//произошла ошибка
		return error;				//вернуть код ошибки
	}//end if

	//создание резервной копии таблицы движений
	tmpTRAFFIC = tmpfile();
	if (!tmpTRAFFIC) {				//файл не найден
		return -2;					//вернуть код ошибки			
	}//end if
	error = cpyFile(-12, TRAFFIC, tmpTRAFFIC);
	if (error != 0) {				//произошла ошибка
		return error;				//вернуть код ошибки
	}//end if

	//получение поставщика
	code = getPost(product);
	if (code < 0) {					//произошла ошибка
		//вернуться к исходной версии файлов
		cpyFile(22, CONTRACTORS, tmpCONTRACTORS);
		return code;				//вернуть код ошибки
	}//end if

	//очистка файла счета на закупку
	error = cleanFile(PURCHASE_INVOICE);
	if (error != 0) {				//произошла ошибка
		//вернуться к исходной версии файлов
		cpyFile(22, CONTRACTORS, tmpCONTRACTORS);
		return error;				//вернуть код ошибки
	}//end if

	//получение товара для закупки
	code = getTovP(product);
	if (code < -1) {				//произошла ошибка
		//вернуться к исходной версии файлов
		cpyFile(22, CONTRACTORS, tmpCONTRACTORS);
		return code;				//вернуть код ошибки
	}//end if

	while (code != -1) {			//пока ввод товаров не закончится
		if (code != 0) {			//пропуск товара с нулевым количеством
			//изменение количества товара на складе
			error = makeStock(1, product);
			if (error != 0) {		//произошла ошибка
				//вернуться к исходной версии файлов
				cpyFile(22, CONTRACTORS, tmpCONTRACTORS);
				cpyFile(20, STORE, tmpSTORE);
				cpyFile(-22, TRAFFIC, tmpTRAFFIC);
				return error;		//вернуть код ошибки
			}//end if

			product.date = today(); //установка текущей даты

			//внесение товара в счет
			error = addToBase(1, product);
			if (error != 0) {		//произошла ошибка
				//вернуться к исходной версии файлов
				cpyFile(22, CONTRACTORS, tmpCONTRACTORS);
				cpyFile(20, STORE, tmpSTORE);
				cpyFile(-22, TRAFFIC, tmpTRAFFIC);
				return error;		//вернуть код ошибки
			}//end if

			//внесение закупки в таблицу движений
			error = addToBase(-2, product);
			if (error != 0) {		//произошла ошибка
				//вернуться к исходной версии файлов
				cpyFile(22, CONTRACTORS, tmpCONTRACTORS);
				cpyFile(20, STORE, tmpSTORE);
				cpyFile(-22, TRAFFIC, tmpTRAFFIC);
				return error;		//вернуть код ошибки
			}//end if
		}//end if

		//получение товара для закупки
		code = getTovP(product);
		if (code < -1) {			//произошла ошибка
			//вернуться к исходной версии файлов
			cpyFile(22, CONTRACTORS, tmpCONTRACTORS);
			cpyFile(20, STORE, tmpSTORE);
			cpyFile(-22, TRAFFIC, tmpTRAFFIC);
			return code;			//вернуть код ошибки
		}//end if
	}//end while

	//проверка наличия новых данных в файле для записи
	error = checkFile(2, PURCHASE_INVOICE);
	if (error != 0) {				//произошла ошибка
		if (error == -2) {			//файл не найден
			//вернуться к исходной версии файлов
			cpyFile(22, CONTRACTORS, tmpCONTRACTORS);
			cpyFile(20, STORE, tmpSTORE);
			cpyFile(-22, TRAFFIC, tmpTRAFFIC);
		}//end if
		return error;				//вернуть код ошибки
	}//end if

	error = printFile(1);			//печать счета на закупку
	return error;					//вернуть код ошибки

}//end purchase()

//продажа товаров
int sale(void) {
	int code;						//ID товара
	int error;						//код ошибки
	FILE* tmpSTORE;					//резервная копия списка товаров
	FILE* tmpTRAFFIC;				//резервная копия таблицы движений
	struct part product;			//товар

	//создание резервной копии списка товаров
	tmpSTORE = tmpfile();
	if (!tmpSTORE) {				//файл не найден
		return -2;					//вернуть код ошибки
	}//end if
	error = cpyFile(10, STORE, tmpSTORE);
	if (error != 0) {				//произошла ошибка
		return error;				//вернуть код ошибки
	}//end if

	//создание резервной копии таблицы движений
	tmpTRAFFIC = tmpfile();
	if (!tmpTRAFFIC) {				//файл не найден
		return -2;					//вернуть код ошибки
	}//end if
	error = cpyFile(-12, TRAFFIC, tmpTRAFFIC);
	if (error != 0) {				//произошла ошибка
		return error;				//вернуть код ошибки
	}//end if

	//проверка наличия данных в файле для чтения
	error = checkFile(1, STORE);
	if (error != 0) {				//произошла ошибка
		return error;				//вернуть код ошибки
	}//end if

	//очистка файла счета на продажу
	error = cleanFile(SALE_INVOICE);
	if (error != 0) {				//произошла ошибка
		return error;				//вернуть код ошибки
	}//end if
	
	//получение товара для продажи
	code = getTovS(product);
	if (code < -1) {				//произошла ошибка
		//вернуться к исходной версии файлов
		cpyFile(20, STORE, tmpSTORE);
		cpyFile(-22, TRAFFIC, tmpTRAFFIC);
		return code;				//вернуть код ошибки
	}//end if
	
	while (code != -1) {			//пока ввод товаров не закончится
		if (code != 0) {			//пропуск товара с нулевым количеством
			//изменение количества товара на складе
			error = makeStock(-1, product);
			if (error != 0) {		//произошла ошибка
				//вернуться к исходной версии файлов
				cpyFile(20, STORE, tmpSTORE);
				cpyFile(-22, TRAFFIC, tmpTRAFFIC);
				return error;		//вернуть код ошибки
			}//end if
						
			product.date = today();//установка текущей даты

			//внесение товара в счет
			error = addToBase(-1, product);
			if (error != 0) {		//произошла ошибка
				//вернуться к исходной версии файлов
				cpyFile(20, STORE, tmpSTORE);
				cpyFile(-22, TRAFFIC, tmpTRAFFIC);
				return error;		//вернуть код ошибки
			}//end if

			//внесение товара в таблицу движений
			error = addToBase(-2, product);
			if (error != 0) {		//произошла ошибка
				//вернуться к исходной версии файлов
				cpyFile(20, STORE, tmpSTORE);
				cpyFile(-22, TRAFFIC, tmpTRAFFIC);
				return error;		//вернуть код ошибки
			}//end if
		}//end if

		//получение товара для продажи
		code = getTovS(product);
		if (code < -1) {			//произошла ошибка
			//вернуться к исходной версии файлов
			cpyFile(20, STORE, tmpSTORE);
			cpyFile(-22, TRAFFIC, tmpTRAFFIC);
			return code;			//вернуть код ошибки
		}//end if
	}//end while

	//проверка наличия новых данных в файле для записи
	error = checkFile(2, SALE_INVOICE);
	if (error != 0) {				//произошла ошибка
		if (error == -2) {			//файл не найден
			//вернуться к исходной версии файлов
			cpyFile(20, STORE, tmpSTORE);
			cpyFile(-22, TRAFFIC, tmpTRAFFIC);
		}//end if
		return error;				//вернуть код ошибки
	}//end if

	error = printFile(-1);			//печать счета на продажу
	return error;					//вернуть код ошибки

}//end sale()

//формирование отчета
int report(void) {
	int error;						//код ошибки
	int mode;						//режим составления отчета
	int ID = 0;						//ID фильтрующего товара/поставщика
	tm begin;						//дата начала отчетного периода
	tm end;							//дата конца отчетного периода

	//проверка наличия данных в файле для чтения
	error = checkFile(1, TRAFFIC);
	if (error != 0) {				//произошла ошибка
		return error;				//вернуть код ошибки
	}//end if

	//получение границ отчетного периода
	error = getPeriod(begin, end);
	if (error == -1) {				//отмена операции
		return -1;					//вернуть код ошибки
	}//end if
	
	//получение режима составления отчета
	mode = getMode(ID);
	if (mode == -1) {				//отмена операции
		return -1;					//вернуть код ошибки
	}//end if
	
	//очистка файла отчета
	error = cleanFile(REPORT);
	if (error != 0) {				//произошла ошибка
		return error;				//вернуть код ошибки
	}//end if
	
	//составление отчета согласно режиму и отчетному периоду
	error = finder(begin, end, mode, ID);
	if (error != 0) {				//произошла ошибка
		return error;				//вернуть код ошибки
	}//end if

	//проверка наличия новых данных в файле для записи
	error = checkFile(2, REPORT);
	if (error != 0) {				//произошла ошибка
		return error;				//вернуть код ошибки
	}//end if

	error = printFile(3);			//печать отчета
	return error;					//вернуть код ошибки

}//end report()

//печать файлов
int print(
	const int& mode) {				//режим выбора группы файлов
	int code;						//номер команды
	int error;						//код ошибки
	int print_mode = 0;				//режим печати
	char s[LEN + 1] = "";			//технологическая строка для печати
	char message[LEN] = "";			//название печатаемого файла
	const char* source = NULL;		//указатель на имя файла-источника
	
	//описание доступных команд
	printf(eng("Доступны режимы:\n", s));
	switch (mode) {
	case 1:							//печать счетов и отчетов	
		printf(eng("1 - печать последнего счета на закупку", s));
		printf(eng("\n2 - печать последнего счета на продажу\n", s));
		printf(eng("3 - печать последнего сформированного", s));
		printf(eng(" отчета\n0 - для отмены операции.\n\nВвод: ", s));
		break;

	case 2:							//печать баз данных
		printf(eng("1 - печать списка товаров на складе", s));
		printf(eng("\n2 - печать списка поставщиков\n", s));
		printf(eng("3 - печать таблицы движений", s));
		printf(eng("\n0 - для отмены операции.\n\nВвод: ", s));
		break;
	}//end switch

	//получение номера команды
	code = getInt(gets_s(s));
	while ((code == INT_MAX) || (code < 0) || (code > 3)) {
		printf(eng("\nВведена недопустимая команда.", s));
		printf(eng(" Повторите попытку.\n\nВвод: ", s));
		code = getInt(gets_s(s));
	}//end while

	//выбор файла-источника, названия файла и режима печати
	switch (code) {
	case 0:							//отмена операции
		return -1;					//вернуть код ошибки
	
	case 1:							//первый вариант из меню
		switch (mode) {
		case 1:						//печать счета на закупку
			source = PURCHASE_INVOICE;
			print_mode = 1;
			sprintf(message, "\t\t\t\t\t\tСЧЕТ НА ЗАКУПКУ");
			break;

		case 2:						//печать списка товаров на складе
			source = STORE;
			print_mode = 0;
			sprintf(message, "\t\t\tСПИСОК ТОВАРОВ НА СКЛАДЕ");
			break;
		}//end switch
		break;

	case 2:							//второй вариант из меню
		switch (mode) {
		case 1:						//печать счета на продажу
			source = SALE_INVOICE;
			print_mode = -1;
			sprintf(message, "\t\t\tСЧЕТ НА ПРОДАЖУ");
			break;

		case 2:						//печать списка поставщиков
			source = CONTRACTORS;
			print_mode = 2;
			sprintf(message, "СПИСОК ПОСТАВЩИКОВ");
			break;
		}//end switch
		break;
	
	case 3:							//третий вариант из меню
		switch (mode) {
		case 1:						//печать отчета
			source = REPORT;
			print_mode = 3;
			sprintf(message, "\t\t\t\t\t\tОТЧЕТ");
			break;

		case 2:						//печать таблицы движений
			source = TRAFFIC;
			print_mode = -2;
			sprintf(message, "\t\t\t\tТАБЛИЦА ПЕРЕМЕЩЕНИЙ");
			break;
		}//end switch
		break;
	}//end switch
	
	//проверка наличия данных в файле для чтения
	error = checkFile(1, source);
	if (error != 0) {				//произошла ошибка
		return error;				//вернуть код ошибки
	}//end if

	//печать названия файла и его содержимого в таблице
	printf("\n\t%s\n", eng(message, s));
	error = printFile(print_mode);

	return error;					//вернуть код ошибки

}//end print()

/*=======================================================================/
/ Обновление базы                                                        /
/=======================================================================*/

//добавление товара в файл
int addToBase(
	const int& mode,				//режим работы
	struct part& product) {			//товар
	int error;						//код ошибки
	const char* source = NULL;		//указатель на имя имя файла-источника
	FILE* base;						//файловый поток для записи
	
	switch (mode) {					//выбор файла-источника
	case 0:							//режим работы со списком товаров
		source = STORE;
		break;

	case 1:							//режим работы со счетом на закупку
		source = PURCHASE_INVOICE;
		break;

	case -1:						//режим работы со счетом на продажу
		source = SALE_INVOICE;
		break;

	case 2:							//режим работы со списком поставщиков
		source = CONTRACTORS;
		break;

	case -2:						//режим работы с таблицей движений
		source = TRAFFIC;
		break;
	}//end switch

	base = fopen(source, "a+b");	//открыть файл для записи в конец файла

	//проверка наличия файла
	if (!base) {					//файл не найден
		return -2;					//вернуть код ошибки
	}//end if

	//запись товара в файл
	error = pasteProduct(base, mode, product);
	if (error != 0) {				//произошла ошибка
		fclose(base);				//закрыть файл
		return error;				//вернуть код ошибки
	}//end if

	fclose(base);					//закрыть файл
	return 0;						//завершение без ошибок

}//end addToBase()

//изменение количества товара на складе
int makeStock(
	const int& mode,				//режим работы
	struct part& product) {			//товар
	int error;						//код ошибки
	long weight;					//объем файла
	FILE* base;						//файловый поток для чтения
	FILE* res;						//файловый поток для записи
	struct part buff;				//товар

	base = fopen(STORE, "rb");		//открыть файл для чтения

	//проверка наличия файла
	if (!base) {					//файл не найден
		return -2;					//вернуть код ошибки
	}//end if

	res = tmpfile();				//создать временный файл

	//проверка наличия файла
	if (!res) {						//файл не найден
		fclose(base);				//закрыть файл
		return -2;					//вернуть код ошибки
	}//end if

	//расчет объема файла
	fseek(base, 0, SEEK_END);
	weight = ftell(base);
	rewind(base);					//вернуться в начало файла
	
	if (weight == 0) {				//файл пустой
		//запись товара в файл
		error = pasteProduct(res, 0, product);
		if (error != 0) {			//произошла ошибка
			fclose(base);			//закрыть файл
			fclose(res);			//закрыть файл
			return error;			//вернуть код ошибки
		}//end if
	}//end if

	while (ftell(base) < weight) {	//пока файл не закончится
		//чтение товара из файла
		error = readProduct(base, 0, buff, ftell(base));
		if (error != 0) {			//произошла ошибка
			fclose(base);			//закрыть файл
			fclose(res);			//закрыть файл
			return error;			//вернуть код ошибки
		}//end if

		//изменение количества искомого товара на складе
		if (buff.tovID == product.tovID) {
			buff.count += product.count;
		}//end if

		//запись товара в файл
		error = pasteProduct(res, 0, buff);
		if (error != 0) {			//произошла ошибка
			fclose(base);			//закрыть файл
			fclose(res);			//закрыть файл
			return error;			//вернуть код ошибки
		}//end if
	}//end while
	
	fclose(base);					//закрыть файл
	error = cpyFile(20, STORE, res);//обновление списка товаров
	fclose(res);					//закрыть файл
	return error;					//вернуть код ошибки

}//end makeStock()

//поиск товаров для составление отчета
int finder(
	const tm& begin,				//дата начала отчетного периода
	const tm& end,					//дата конца отчетного периода
	const int &mode, 				//режим работы
	const int& ID) {				//ID фильтрующего товара/поставщика
	int error;						//код ошибки
	long weight;					//объем файла
	bool found;						//результат поиска товара по параметрам
	FILE* report;					//файловый поток для записи
	FILE* moves;					//файловый поток для чтения
	struct part product;			//товар
	
	//проверка наличия данных в файле для чтения
	error = checkFile(0, TRAFFIC);
	if (error != 0) {				//произошла ошибка
		return error;				//вернуть код ошибки
	}//end if

	moves = fopen(TRAFFIC, "rb");	//открыть файл для чтения
	
	report = fopen(REPORT, "a+b");	//открыть файл для записи в конец

	//проверка наличия файла
	if (!report) {					//файл не найден
		fclose(moves);				//закрыть файл
		return -2;					//вернуть код ошибки
	}//end if

	//расчет объема файла
	fseek(moves, 0, SEEK_END);
	weight = ftell(moves);
	rewind(moves);					//вернуться в начало файла

	while (ftell(moves) < weight) {	//пока файл не закончится
		//чтение товара из файла
		error = readProduct(moves, -2, product, ftell(moves));
		if (error != 0) {			//произошла ошибка
			fclose(report);			//закрыть файл
			fclose(moves);			//закрыть файл
			return error;			//вернуть код ошибки
		}//end if

		//относится ли дата закупки/продажи товара к отчетному периоду
		found = cmpDate(begin, product.date) && 
				cmpDate(product.date, end);

		if (found) {				//дата подходящая
			switch (mode) {			//фильтрация по товару/поставщику
			case 10:				//отчет по закупкам одного товара
				found = found && 
						(product.tovID == ID) && (product.count > 0);
				break;

			case 12:				//отчет по закупкам одного поставщика
				found = found && 
						(product.postID == ID) && (product.count > 0);
				break;

			case 1:					//отчет по закупкам всех товаров
				found = found && (product.count > 0);
				break;

			case 20:				//отчет по продажам одного товара
				found = found && 
						(product.tovID == ID) && (product.count < 0);
				break;

			case 22:				//отчет по продажам одного поставщика
				found = found && 
						(product.postID == ID) && (product.count < 0);
				break;

			case 2:					//отчет по продажам всех товаров
				found = found && (product.count < 0);
				break;
			}//end switch

			if (found) {			//товар подходящий
				//запись товара в файл
				error = pasteProduct(report, 3, product);
				if (error != 0) {	//произошла ошибка
					fclose(report);	//закрыть файл
					fclose(moves);	//закрыть файл
					return error;	//вернуть код ошибки
				}//end if
			}//end if
		}//end if
	}//end while

	fclose(report);					//закрыть файл
	fclose(moves);					//закрыть файл
	return 0;						//завершение без ошибок

}//end finder()

/*=======================================================================/
/ Инструменты для получения информации от пользователя                   /
/=======================================================================*/

//получение поставщика
int getPost(
	struct part& product) {			//товар
	int error;						//код ошибки
	int code;						//номер команды и ID поставщика
	int found1;						//наличие ID поставщика в базе
	int found2;						//наличие названия поставщика в базе
	char s[LEN + 1] = "";			//технологическая строка для печати
	char name[MAX_POST + 1] = "";	//название поставщика

	//описание доступных команд
	printf(eng("Введите одну из команд:\nID одного из поставщиков -", s));
	printf(eng(" для выбора этого поставщика\n-1 - для добавления", s));
	printf(eng(" нового поставщика\n 0 - для отмены\n", s));
	printf(eng("\nСписок доступных поставщиков:\n", s));
	
	//печать списка поставщиков
	error = printList(2, 0);
	if (error != 0) {				//произошла ошибка
		return error;				//вернуть код ошибки
	}//end if
	printf(eng("Ввод: ", s));
	
	do {							//пока введенный ID не найден в базе
		//получение номера команды или ID поставщика
		code = getInt(gets_s(s));
		while ((code == INT_MAX) || (code < -1)) {
			printf(eng("\nВведен некорректный ID.", s));
			printf(eng(" Повторите попытку.\n\nВвод: ", s));
			code = getInt(gets_s(s));
		}//end while

		if (code == 0) {			//отмена операции
			return -1;				//вернуть код ошибки
		}//end if

		//добавление нового поставщика
		if (code == -1) {
			printf(eng("\nДобавление нового поставщика.", s));
			printf(eng("\nВведите новое название поставщика", s));
			printf("%s%d", eng(" не длиннее ", s), MAX_POST);
			printf(eng(" символов. Введите 0 для отмены.\n\nВвод: ", s));

			//пока не введено название поставщика, не встречающееся в базе
			do {
				//получение имени товара
				gets_s(name);
				while ((checkString(name) == false) || 
					   (strlen(name) > MAX_POST)) {
					printf(eng("\nВведено название некорректной ", s));
					printf(eng("длины. Повторите попытку.\n\nВвод: ", s));
					gets_s(name);
				}//end while
								
				if (getInt(name) == 0) {//отмена операции
					return -1;			//вернуть код ошибки
				}//end if

				//дополнение длины названия до стандартной
				sprintf(name, "%-*s", MAX_POST, name);

				//поиск названия в списке поставщиков
				found2 = checkExistence(2, 0, name);
				if (found2 == 1) {
					printf(eng("\nВведенное название поставщика", s));
					printf(eng(" уже существует. Повторите", s));
					printf(eng(" попытку.\n\nВвод: ", s));
				} else if (found2 < 0) {//произошла ошибка
					return found2;		//вернуть код ошибки
				}//end if

			} while (found2 == 1);//end while

			//запись назвния поставщика в структуру
			strncpy(product.postName, name, MAX_POST);

			//выделение нового ID поставщика
			code = newID(2);
			if (code <= 0) {		//произошла ошибка
				return code;		//вернуть код ошибки
			}//end if

			//запись ID нового поставщика в структуру
			product.postID = code;
			
			//добавление нового поставщика в список поставщиков
			error = addToBase(2, product);
			if (error != 0) {		//произошла ошибка
				return error;		//вернуть код ошибки
			}//end if

			printf(eng("\nНовый поставщик успешно добавлен.\n", s));
		}//end if

		//поиск введенного ID поставщика в базе
		found1 = checkExistence(2, code, "");
		if (found1 == 0) {
			printf(eng("\nВведенный ID не найден. Повторите", s));
			printf(eng(" попытку.\nВведите -1 для добавления", s));
			printf(eng("нового поставщика.\n\nВвод: ", s));
		} else if (found1 < 0) {	//произошла ошибка
			return found1;			//вернуть код ошибки
		}//end if

	} while (found1 == 0);//end while
		
	product.postID = code;			//запись ID поставщика в структуру
	return 0;						//завершение без ошибок

}//end getPost()

//получение товара для закупки
int getTovP(
	struct part& product) {			//товар
	int error;						//код ошибки
	int ID;							//ID поставщика
	int code;						//номер команды и ID товара
	int found1;						//наличие ID товара в базе
	int found2;						//наличие названия товара в базе
	char s[LEN + 1] = "";			//технологическая строка для печати
	char name[MAX_TOV + 1] = "";	//название товара

	//описание доступных команд
	printf(eng("Введите одну из команд:\nID одного из товаров - ", s));
	printf(eng("для выбора этого товара\n-1 - для добавления нового", s));
	printf(eng(" товара\n 0 - для завершения ввода\n", s));
	printf(eng("\nСписок доступных товаров:\n", s));
	
	//печать списка товаров
	error = printList(0, product.postID);
	if (error != 0) {				//произошла ошибка
		return error;				//вернуть код ошибки
	}//end if
	printf(eng("Ввод: ", s));
		
	do {							//пока введенный ID не найден в базе
		//получение номера команды или ID товара
		code = getInt(gets_s(s));
		while ((code == INT_MAX) || (code < -1)) {
			printf(eng("\nВведен некорректный ID.", s));
			printf(eng("Повторите попытку.\n\nВвод: ", s));
			code = getInt(gets_s(s));
		}//end while

		if (code == 0) {			//завершение ввода
			printf("\n");
			return -1;				//вернуть код ошибки
		}//end if

		//добавление нового товара
		if (code == -1) {
			printf(eng("\nДобавление нового товара.\nВведите новое", s));
			printf(eng(" название товара не длиннее", s));
			printf(" %d %s", MAX_TOV, eng("символов.\n\nВвод: ", s));
			
			//пока не введено название товара, не встречающееся в базе
			do {
				//получение имени товара
				gets_s(name);
				while ((checkString(name) == false) 
					|| (strlen(name) > MAX_TOV)) {
					printf(eng("\nВведено название некорректной ", s));
					printf(eng("длины. Повторите попытку.\n\nВвод: ", s));
					gets_s(name);
				}//end while

				//дополнение длины названия до стандартной
				sprintf(name, "%-*s", MAX_TOV, name);

				//поиск названия в списке товарров
				found2 = checkExistence(0, 0, name);
				if (found2 == 1) {
					printf(eng("\nВведенное название товара уже", s));
					printf(eng(" существует. Повторите попытку.", s));
					printf(eng("\n\nВвод: ", s));
				} else if (found2 < 0) {//произошла ошибка
					return found2;		//вернуть код ошибки
				}//end if
			} while (found2 == 1);//end while

			//запись имени товара в структуру
			strncpy(product.tovName, name, MAX_TOV);
			
			//получение цены товара
			printf(eng("\nВведите цену товара, являющуюся", s));
			printf(eng(" положительным числом.\n\nВвод: ", s));
			code = getInt(gets_s(s));
			while ((code == INT_MAX) || (code <= 0)) {
				printf(eng("\nВведена некорректная цена.", s));
				printf(eng(" Введите число.\n\nВвод: ", s));
				code = getInt(gets_s(s));
			}//end while
			product.cost = code;	//заись цены товара в структуру

			//выделение нового ID товара
			code = newID(0);
			if (code <= 0) {		//произошла ошибка
				return code;		//вернуть код ошибки
			}//end if
						
			product.tovID = code;	//заись ID новго товара в структуру
			product.count = 0;		//обнуление количества товара

			//добавление нового товара в список товаров
			error = addToBase(0, product);
			if (error != 0) {		//произошла ошибка
				return error;		//вернуть код ошибки
			}//end if

			printf(eng("\nНовый товар успешно добавлен.\n", s));
		}//end if

		//поиск введенного ID товара в базе
		found1 = checkExistence(0, code, "");
		if (found1 == 0) {
			printf(eng("\nВведенный ID не найден. Повторите", s));
			printf(eng(" попытку.\nВведите -1 для добавления", s));
			printf(eng(" нового товара.\n\nВвод: ", s));
		} else if (found1 < 0) {	//произошла ошибка
			return found1;			//вернуть код ошибки
		}//end if

		//поиск данных товара по ID
		ID = product.postID;
		error = fillProduct(code, product);
		if (error != 0) {			//произошла ошибка
			return error;			//вернуть код ошибки
		}//end if

		//проверка соответствия ID товара и поставщика
		if (ID != product.postID) {	//товар связан с другим поставщиком
			printf(eng("\nВыбранный ID товара не связан с ранее ", s));
			printf(eng("введенным ID поставщика. повторите попытку.", s));
			printf(eng("\nВведите 0 для завершения ввода.\n\nВвод: ", s));
			found1 = 0;				//обнуление результата ввода
			product.postID = ID;	//возвращение к исходному поставщику
		}//end if
	} while (found1 == 0);//end while

	//получение количества товара
	printf(eng("\nВведите количество полученного товара, являющееся", s));
	printf(eng(" положительным числом.\n", s));
	printf(eng("Введите 0 для отмены продажи товара.\n\nВвод: ", s));
	code = getInt(gets_s(s));

	//отмена закупки товара
	if (code == 0) {				//пропуск ввода товара
		return 0;					//вернуть код ошибки
	}//end if

	//пока не введено корректное количество
	while ((code == INT_MAX) || (code <= 0)) {
		//получение количества товара
		printf(eng("\nВведено некорректое количество.", s));
		printf(eng(" Введите положительное число.\n", s));
		printf(eng("Введите 0 для отмены продажи товара.\n\nВвод: ", s));
		code = getInt(gets_s(s));

		//отмена закупки товара
		if (code == 0) {			//пропуск ввода товара
			return 0;				//вернуть код ошибки
		}//end if
	}//end while
		
	product.count = code;			//запись количества товара в структуру
	return code;					//вернуть ID товара

}//end getTovP()

//получение товара для продажи
int getTovS(
	struct part& product) {			//товар
	int error;						//код ошибки
	int code;						//номер команды и ID товара
	int found;						//наличие ID товара в базе
	char s[LEN + 1] = "";			//технологическая строка для печати

	//описание доступных команд
	printf(eng("Введите одну из команд:\nID одного из товаров -", s));
	printf(eng(" для выбора этого товара\n0 - для завершения ввода", s));
	printf(eng("\n\nСписок доступных товаров:\n", s));

	//печать списка товаров
	error = printList(0, 0);
	if (error != 0) {				//произошла ошибка
		return error;				//вернуть код ошибки
	}//end if
	printf(eng("Ввод: ", s));

	do {							//пока введенный ID не найден в базе
		//получение номера команды или ID товара
		code = getInt(gets_s(s));
		while ((code == INT_MAX) || (code < 0)) {
			printf(eng("\nВведен некорректный ID.", s));
			printf(eng(" Введите число.\n\nВвод: ", s));
			code = getInt(gets_s(s));
		}//end while

		if (code == 0) {			//завершение ввода
			printf("\n");
			return -1;				//вернуть код ошибки
		}//end if

		//поиск введенного ID товара в базе
		found = checkExistence(0, code, "");
		if (found == 0) {
			printf(eng("\nВведенный ID не найден.", s));
			printf(eng(" Повторите попытку.\n\nВвод: ", s));
		} else if (found < 0) {		//произошла ошибка
			return found;			//вернуть код ошибки
		}//end if
	} while (found == 0);//end while

	//поиск данных товара по ID
	error = fillProduct(code, product);
	if (error != 0) {				//произошла ошибка
		return error;				//вернуть код ошибки
	}//end if

	//проверка наличия товара
	if (product.count == 0) {		//товар закончился
		printf(eng("\nВыбранный товар закончился. Выберите другой", s));
		printf(eng(" товар или завершите ввод товаров.\n\nВвод: ", s));
		return 0;					//вернуть код ошибки
	}//end if

	//получение количества товара
	printf("%s%d", eng("\nДоступно ", s), product.count);
	printf(eng(" единиц указанного товара.\nВведите количество", s));
	printf(eng(" товара для продажи.\nВведите 0 для отмены", s));
	printf(eng(" продажи товара.\n\nВвод: ", s));
	code = getInt(gets_s(s));

	//отмена продажи товара
	if (code == 0) {				//пропуск ввода товара
		return 0;					//вернуть код ошибки
	}//end if

	//пока не введено корректное количество
	while ((code == INT_MAX) || (code < 0) || (code > product.count)) {
		//получение количества товара
		printf(eng("\nВведено некорректое количество.\nВведите", s));
		printf("%s%d.\n", eng(" положительное число, не превышающее ", s),
			product.count);
		printf(eng("Введите 0 для отмены продажи товара.\n\nВвод: ", s));
		code = getInt(gets_s(s));

		//отмена продажи товара
		if (code == 0) {			//пропуск ввода товара
			return 0;				//вернуть код ошибки
		}//end if
	}//end while

	product.count = -code;			//запись количества товара в структуру	
	return code;					//вернуть ID товара

}//end getTovS()

//получение отчетного периода
int getPeriod(
	tm& begin, 						//дата начала отчетного периода
	tm& end) {						//дата конца отчетного периода
	char buff[LEN + 1] = "";		//строка, содержащая дату
	char s[LEN + 1] = "";			//технологическая строка для печати
	
	//описание доступных команд
	printf(eng("Введите начало и конец рассматриваемого периода\n", s));
	printf(eng("в формате <день>.<месяц>.<год>, напимер, 01.10.2020", s));
	printf(eng("\nВведите дату начала периода в правильном формате.", s));
	printf(eng("\nВведите -1 для выбора текущей даты в качестве", s));
	printf(eng(" начала периода.\nВведите 0 для отмены.\n\nВвод: ", s));

	//обнуление даты начала периода
	begin.tm_year = begin.tm_mon = begin.tm_mday = 0;
	end = today();					//установка даты конца периода
	
	//пока не будет получена корректная дата начала периода
	do {
		do {						//пока формат даты некорректный
			gets_s(buff);			//получение даты в строке

			if (getInt(buff) == 0) {//отмена операции
				return -1;			//вернуть код ошибки
			}//end if

			//установка текущей даты в качестве даты начала периода
			if (getInt(buff) == -1) {
				begin = today();
				break;
			}//end if

			begin = convertDate(buff);//перевод даты из строки в структуру
			
			if (begin.tm_year == -1) {//введена дата некорректного формата
				printf(eng("\nВведена дата некорректного формата.", s));
				printf(eng(" Повторите попытку.\nТребуется ввести", s));
				printf(eng(" дату в формате <день>.<месяц>.<год>.\n", s));
				printf(eng("Введите 0 для отмены.\n\nВвод: ", s));
			}//end if
		} while (begin.tm_year == -1);//end while

		if (!cmpDate(begin, end)) {	//дата начала периода позже текущей
			printf(eng("\nВведена недопустимая дата.\nВведите дату", s));
			printf(eng(" не позднее сегодняшнего дня.\n\nВвод: ", s));
		}//end if
	} while (!cmpDate(begin, end));//end while

	//получение даты конца периода
	printf(eng("\nВведите дату конца периода в правильном формате.", s));
	printf(eng("\nВведите -1 для выбора текущей даты в качестве", s));
	printf(eng(" конца периода.\nВведите 0 для отмены.\n\nВвод: ", s));

	//обнуление даты конца периода
	end.tm_year = end.tm_mon = end.tm_mday = 0;
	
	//пока не будет получена корректная дата конца периода
	do {
		do {						//пока формат даты некорректный
			gets_s(buff);			//получение даты в строке
			
			if (getInt(buff) == 0) {//отмена операции
				return -1;			//вернуть код ошибки
			}//end if
			
			//установка текущей даты в качестве даты конца периода
			if (getInt(buff) == -1) {
				end = today();
				break;
			}//end if
			
			end = convertDate(buff);//перевод даты из строки в структуру

			if (end.tm_year == -1) {//введена дата некорректного формата
				printf(eng("\nВведена дата некорректного формата.", s));
				printf(eng(" Повторите попытку.\nТребуется ввести", s));
				printf(eng(" дату в формате <день>.<месяц>.<год>.\n", s));
				printf(eng("Введите 0 для отмены.\n\nВвод: ", s));
			}//end if
		} while (end.tm_year == -1);//end while

		if (!cmpDate(begin, end)) {	//дата начала периода позже даты конца
			printf(eng("\nВведена недопустимая дата.\nВведите дату", s));
			printf(eng(" не позднее начала периода.\n\nВвод: ", s));
		}//end if
	} while (!cmpDate(begin, end));//end while

	return 0;						//завершение без ошибок

}//end getPeriod()

//получение режима составления отчета
int getMode(
	int& code) {					//ID фильтрующего товара/поставщика
	int error;						//код ошибки
	int found;						//наличие ID товара/поставщика в базе
	int mode1;						//режим отчета
	int mode2;						//режим отчета
	char s[LEN + 1] = "";			//технологическая строка для печати

	//описание доступных команд
	printf(eng("\nДля формирования отчета доступны режимы:\n1 -", s));
	printf(eng(" отчет по закупкам\n2 - отчет по продажам\n0 -", s));
	printf(eng(" отмена операции.\n\nВвод: ", s));

	//получение номера режима отчета
	mode1 = getInt(gets_s(s));
	while ((mode1 == INT_MAX) || (mode1 < 0) || (mode1 > 2)) {
		printf(eng("\nВведена недопустимая команда.", s));
		printf(eng(" Повторите попытку.\n\nВвод: ", s));
		mode1 = getInt(gets_s(s));
	}//end while
	
	//описание доступных команд
	printf(eng("\nДоступна фильтрация данных:\n1 - по операциям", s));
	printf(eng(" с одним поставщиком\n2 - по операциям с одним", s));
	printf(eng(" товаром\n3 - по всем операциям за указанный", s));
	printf(eng(" период\n0 - отмена операции.\n\nВвод: ", s));

	//получение номера режима фильтрации данных
	mode2 = getInt(gets_s(s));
	while ((mode2 == INT_MAX) || (mode2 < 0) || (mode2 > 3)) {
		printf(eng("\nВведена недопустимая команда.", s));
		printf(eng(" Повторите попытку.\n\nВвод: ", s));
		mode2 = getInt(gets_s(s));
	}//end while

	//установка режима фильтрации данных
	//получение фильтрующего товара/поставщика
	switch (mode2) {
	case 1:							//режим с фильтрующим поставщиком
		mode2 = 2;
		printf(eng("\nВведите:\nID одного из поставщиков - для", s));
		printf(eng(" выбора этого поставщика\n0 - для отмены.\n", s));
		printf(eng("\nСписок доступных поставщиков:\n", s));
		break;

	case 2:							//режим с фильтрующим товаром
		mode2 = 0;
		printf(eng("\nВведите:\nID одного из товаров - для выбора ", s));
		printf(eng("этого товара\n0 - для отмены.\n\nСписок доступ", s));
		printf(eng("ных товаров:\n", s));
		break;

	case 3:							//режим без фильтра
		mode2 = 0;
		printf("\n");
		return mode1;				//вернуть режим печати

	case 0:							//отмена операции
		return -1;					//вернуть код ошибки
	}//end switch

	mode1 = mode1 * 10 + mode2;		//установка результирующего режима

	//печать списка товаров/поставщиков
	error = printList(mode2, 0);
	if (error != 0) {				//произошла ошибка
		return error;				//вернуть код ошибки
	}//end if
	printf(eng("Ввод: ", s));

	do {							//пока введенный ID не найден в базе
		//получение ID фильтрующего товара/поставщика
		code = getInt(gets_s(s));
		while ((code == INT_MAX) || (code < 0)) {
			printf(eng("\nВведен некорректный ID.", s));
			printf(eng(" Введите число.\n\nВвод: ", s));
			code = getInt(gets_s(s));
		}//end while

		if (code == 0) {			//отмена операции
			return -1;				//вернуть код ошибки
		}//end if

		//поиск введенного ID товара/поставщика в базе
		found = checkExistence(mode2, code, "");
		if (found == 0) {
			printf(eng("\nВведенный ID не найден.", s));
			printf(eng(" Повторите попытку.\n\nВвод: ", s));
		} else if (found < 0) {		//произошла ошибка
			return found;			//вернуть код ошибки
		}//end if
	} while (found == 0);//end while
	printf("\n");

	return mode1;					//вернуть режим печати

}//end getMode()

/*=======================================================================/
/ Инструменты для поиска данных                                          /
/=======================================================================*/

//поиск  всех данных товара по ID
int fillProduct(
	const int& tovID, 
	struct part& product) {			//товар
	int fID;						//полученный из файла ID
	int error;						//код ошибки
	long weight;					//объем файла
	long pos;						//отступ в файле
	void* ptr;						//указатель на имя буфер для чтения
	FILE* base;						//файловый поток для чтения
		
	//проверка наличия данных в файле для чтения
	error = checkFile(0, STORE);
	if (error != 0) {				//произошла ошибка
		return error;				//вернуть код ошибки
	}//end if

	base = fopen(STORE, "rb");		//открыть файл для чтения

	//расчет объема файла
	fseek(base, 0, SEEK_END);
	weight = ftell(base);
	rewind(base);					//вернуться в начало файла
	
	while (ftell(base) < weight) {	//пока файл не закончится
		//чтение ID товара из файла
		ptr = &fID;
		error = fread(ptr, sizeof(int), 1, base);
		if (error < 1) {			//ошибка чтения из файла
			fclose(base);			//закрыть файл
			return -4;				//вернуть код ошибки
		}//end if

		if (fID == tovID) {			//ID из файла совпадает с искомым
			//возвращение к началу строки в файле
			pos = ftell(base) - sizeof(int);
			fseek(base, pos, SEEK_SET);

			//чтение товара из файла
			error = readProduct(base, 0, product, ftell(base));
			if (error != 0) {		//произошла ошибка
				fclose(base);		//закрыть файл
				return error;		//вернуть код ошибки
			}//end if

			//поиск имени поставщика по ID
			error = findName(2, product.postID, product.postName);
			
			fclose(base);			//закрыть файл
			return error;			//вернуть код ошибки
		} else {
			//пропуск строки, содержащей неподходящий товар
			pos = sizeof(char) * MAX_TOV + sizeof(int) * 3;
			fseek(base, pos, SEEK_CUR);
		}//end if
	}//end while

	fclose(base);					//закрыть файл
	return -4;						//ошибка чтения из файла

}//end fillProduct()

//поиск имени товара по ID
int findName(
	const int& mode, 				//режим работы
	const int& ID,					//ID товара/поставщика
	char(*name)) {					//название товара/поставщика
	int error;						//код ошибки
	long weight;					//объем файла
	long strweight = 0;				//длина строки в байтах
	const char* source = NULL;		//указатель на имя файла-источника
	FILE* base;						//файловый поток для чтения
	struct part product;			//товар

	//выбор файла-источника и расчет длины строки
	switch (mode) {
	case 2:							//режим работы со списком поставщиков
		source = CONTRACTORS;
		strweight = sizeof(int) + sizeof(char) * MAX_POST;
		break;

	case 0:							//режим работы со списком товаров
		source = STORE;
		strweight = sizeof(int) * 4 + sizeof(char) * MAX_TOV;
		break;
	}//end switch

	//проверка наличия данных в файле для чтения
	error = checkFile(0, source);
	if (error != 0) {				//произошла ошибка
		return error;				//вернуть код ошибки
	}//end if

	base = fopen(source, "rb");		//открыть файл для чтения

	//расчет объема файла
	fseek(base, 0, SEEK_END);
	weight = ftell(base);
	rewind(base);					//вернуться в начало файла

	while (ftell(base) < weight) {	//пока файл не закончится
		//чтение товара из файла
		error = readProduct(base, mode, product, ftell(base));
		if (error != 0) {			//произошла ошибка
			fclose(base);			//закрыть файл
			return error;			//вернуть код ошибки
		}//end if

		//проверка совпадения полученного ID с искомым
		//запись названия искомого товара/поставщика
		switch (mode) {				
		case 0:						//режим работы со списком товаров
			if (ID == product.tovID) {
				strncpy(name, product.tovName, MAX_TOV);
				return 0;			//завершение без ошибок
			}//end if
			break;

		case 2:						//режим работы со списком поставщиков
			if (ID == product.postID) {
				strncpy(name, product.postName, MAX_POST);
				return 0;			//завершение без ошибок
			}//end if
			break;
		}//end switch
	}//end while
	
	fclose(base);					//закрыть файл
	return -4;						//имя товара/поставщика не найдено

}//end findName()

//проверка существования ID или названия в базе
int checkExistence(
	const int& mode, 				//режим работы
	const int& ID,					//ID товара/поставщика
	const char(*name)) {			//название товара/поставщика
	int error;						//код ошибки
	long weight;					//объем файла
	long strweight = 0;				//длина строки в байтах
	const char* source = NULL;		//указатель на имя файла-источника
	FILE* base;						//файловый поток для чтения
	struct part product;			//товар
	
	//выбор файла источника и расчет длины строки файла
	switch (mode) {
	case 2:							//режим работы со списком поставщиков
		source = CONTRACTORS;
		strweight = sizeof(int) + sizeof(char) * MAX_POST;
		break;

	case 0:							//режим работы со списком товаров
		source = STORE;
		strweight = sizeof(int) * 4 + sizeof(char) * MAX_TOV;
		break;
	}//end switch

	base = fopen(source, "rb");		//открыть файл для чтения

	//проверка наличия файла
	if (!base) {					//файл не найден
		return -2;					//вернуть код ошибки
	}//end if

	//расчет объема файла
	fseek(base, 0, SEEK_END);
	weight = ftell(base);
	rewind(base);					//вернуться в начало файла
	
	if (weight == 0) {				//файл пустой
		fclose(base);				//закрыть файл
		return 0;					//товар/поставщик не найден
	}//end if
	
	while (ftell(base) < weight) {	//пока файл не закончится
		//обнуление ID товара и поставщика
		product.tovID = 0;
		product.postID = 0;

		//чтение товара из файла
		error = readProduct(base, mode, product, ftell(base));
		if (error != 0) {				//произошла ошибка
			fclose(base);			//закрыть файл
			return error;			//вернуть код ошибки
		}//end if

		//проверка совпадения полученных ID или названия с искомыми
		switch (mode) {
		case 0:						//режим работы со списком товаров
			if ((ID == product.tovID) ||
				(strcmp(name, product.tovName) == 0)) {
				return 1;			//товар/поставщик найден
			}//end if
			break;

		case 2:						//режим работы со списком поставщиков
			if ((ID == product.postID) ||
				(strcmp(name, product.postName) == 0)) {
				return 1;			//товар/поставщик найден
			}//end if
			break;
		}//end switch
	}//end while

	fclose(base);					//закрыть файл
	return 0;						//товар/поставщик не найден

}//end checkExistence()

//выделение нового ID
int newID(
	const int& mode) {				//режим работы
	int lastID;						//ID последнего товара/поставщика
	int error;						//код ошибки
	long pos;						//отступ в файле
	long weight;					//объем файла
	long strweight = 0;				//длина строки в байтах
	const char* source = NULL;		//указатель на имя файла-источника
	void* ptr;						//указатель на буфер для чтения
	FILE* base;						//файловый поток для чтения
	
	//выбор файла-источника и расчет длины строки
	switch (mode) {
	case 2:							//режим работы со списком поставщиков
		source = CONTRACTORS;
		strweight = sizeof(int) + sizeof(char) * MAX_POST;
		break;

	case 0:							//режим работы со списком товаров
		source = STORE;
		strweight = sizeof(int) * 4 + sizeof(char) * MAX_TOV;
		break;
	}//end switch

	base = fopen(source, "rb");		//открыть файл для чтения
	
	//проверка наличия файла
	if (!base) {					//файл не найден
		return -2;					//вернуть код ошибки
	}//end if

	//расчет объема файла
	fseek(base, 0, SEEK_END);
	weight = ftell(base);
	
	if (weight == 0) {				//файл пустой
		fclose(base);				//закрыть файл
		return 1;					//вернуть новый ID
	}//end if

	//переход к последней строке файла
	pos = weight - strweight;
	fseek(base, pos, SEEK_SET);

	//чтение ID последнего товара из файла
	ptr = &lastID;
	error = fread(ptr, sizeof(int), 1, base);
	fclose(base);					//закрыть файл
	if (error < 1) {				//ошибка чтения из файла
		return -4;					//вернуть код ошибки
	}//end if

	return lastID++;				//вернуть новый ID

}//end newID()

/*=======================================================================/
/ Инструменты для работы с датами                                        /
/=======================================================================*/

//перевод даты из строки в структуру
tm convertDate(
	const char(*word)) {			//строка, содержащая дату
	int i;							//счетчик
	bool error;						//результат проверки корректности даты
	char digit[5] = "";				//буфер для хранения числа/месяца/года
	tm date;						//структура для записи даты

	//требуемый формат даты: --.--.----

	//длина строки должна быть равна 10
	error = strlen(word) == 10;
	//проверка наличия точек-разделителей
	error = (error) && (word[2] == '.');
	error = (error) && (word[5] == '.');
	//проверка наличия символов, не являющихся цифрами, помимо точек
	for (i = 0; i < 10; i++) {
		if ((i != 2) && (i != 5)) {
			error = (error) && (isdigit(word[i]));
		}//end if
	}//end for i
		
	if (error) {					//дата корректного формата
		//перевод числа из строки в структуру
		for (i = 0; i < 2; i++) {	
			digit[i] = word[i];
		}//end for i
		date.tm_mday = atoi(digit);
		
		//перевод месяца из строки в структуру
		for (i = 3; i < 5; i++) {	
			digit[i - 3] = word[i];
		}//end for i
		date.tm_mon = atoi(digit);

		//перевод года из строки в структуру
		for (i = 6; i < 10; i++) {
			digit[i - 6] = word[i];
		}//end for i
		date.tm_year = atoi(digit);
	} else {						//дата некорректного формата	
		date.tm_year = -1;
	}//end if
	
	return date;					//вернуть дату в структуре

}//end convertDate()

//определение текущей даты
tm today(void) {
	tm* date;						//структура для хранения текущей даты
	time_t current;					//текущее время в секундах

	//расчет текущего времени и перевод в структуру
	time(&current);
	date = localtime(&current);

	//корректировка даты
	date->tm_year += 1900;
	date->tm_mon += 1;

	return *date;					//вернуть текущую дату в структуре

}//end today()

//проверка утверждения, что первая дата не позже второй
bool cmpDate(
	const tm& first,				//первая дата
	const tm& second) {				//вторая дата

	if (second.tm_year < first.tm_year) {
		return false;				//вторая дата раньше первой
	} else if (second.tm_year > first.tm_year) {
		return true;				//первая дата раньше второй
	} else {
		if (second.tm_mon < first.tm_mon) {
			return false;			//вторая дата раньше первой
		} else if (second.tm_mon > first.tm_mon) {
			return true;			//первая дата раньше второй
		} else {
			if (second.tm_mday < first.tm_mday) {
				return false;		//вторая дата раньше первой
			} else {
				return true;		//первая дата раньше второй
			}//end if
		}//end if
	}//end if

}//end cmpDate()

/*=======================================================================/
/ Вспомогательные инструменты                                            /
/=======================================================================*/

//перевод строки в кодировку 1251
char* eng(
	const char(*rus),				//исходная строка
	char(*res)) {					//строка в новой кодировке
	int i;							//счетчик
	int leng;						//длина строки
	int symb;						//код символа

	strncpy(res, rus, LEN);			//создание копии исходной строки

	leng = strlen(rus);				//расчет длины строки

	//перевод строки из русской кодировки в английскую
	for (i = 0; i < leng; i++) {
		symb = (unsigned char)(rus[i]);//получение кода i-го символа

		//перевод символа в кодировку 1251
		if ((symb > 191) && (symb < 240)) {
			//смещение кода для символов от "А" до "п"
			res[i] = (unsigned char)(symb - 64);
		} else if ((symb > 239) && (symb < 256)) {
			//смещение кода для символов от "р" до "я"
			res[i] = (unsigned char)(symb - 16);
		} else if (symb == 168) {
			//смещение кода для символа "Ё"
			res[i] = (unsigned char)(symb + 72);
		} else if (symb == 184) {
			//смещение кода для символа "ё"
			res[i] = (unsigned char)(symb + 57);
		}//end if
	}//end for i

	res[i] = '\0';					//добавление символа конца строки
	return res;						//вернуть пееведенную строку

}//end eng()

//проверка строки на наличие букв или цифр
bool checkString(
	const char(*string)) {			//строка для проверки
	int i, j;						//счетчики
	int leng;						//длина строки
	int symb;						//код символа
	//массив интервалов кодов, в которых находятся буквы и цифры
	int sgm[10] = { 48, 57, 65, 90, 97, 122, 128, 175, 224, 241 };

	leng = strlen(string);			//расчет длины строки

	for (j = 0; j < leng; j++) {	//посимвольная проверка строки
		symb = (unsigned char)string[j];//получение кода i-го символа

		//рассмотрение 5 разрешенных интервалов 
		for (i = 0; i <= 8; i += 2) {
			if (symb < sgm[i]) {	//код слева от разрешенного интервала
				break;
			} else if (symb <= sgm[i + 1]) {//код в разрешенном интервале
				return true;		//в строке найдены буквы или цифры
			}//end if
		}//end for i
	}//end for j
	
	return false;					//в строке не найдены буквы или цифры
	
}//end checkString()

//получение числа из строки
int getInt(
	const char(*string)) {			//строка, содержащая число
	int digit;						//считываемое число
	char* word;						//строка, содержащая число
	char copy[LEN + 1] = "";		//копия исходной строки

	strncpy(copy, string, LEN);		//создание копии исходной строки
	
	//удаление ведущих пробельных символов и поиск первого слова в строке
	word = strtok(copy, "\t \n");

	if (word == NULL) {				//слово не найдено
		return INT_MAX;				//вернуть код ошибки
	}//end if

	digit = atoi(word);				//перевод слова в число

	//проверка наличия букв в слове
	if (digit == 0) {
		
		if (word[0] == '0') {		//слово содержит число "0"
			return 0;				//вернуть число
		} else {					//слово содержит буквы
			return INT_MAX;			//вернуть код ошибки
		}//end if
	}//end if

	return digit;					//вернуть число

}//end getInt()

//копирование содержимого одного файла в другой
int cpyFile(
	const int& mode, 				//режим работы
	const char(*src_file),			//имя файла-источника
	FILE* temp) {					//временный файловый поток
	int error;						//код ошибки
	long weight;					//объем файла
	long strweight = 0;				//длина строки в байтах
	struct part product;			//товар
	FILE* source = NULL;			//файловый поток для чтения
	FILE* dest = NULL;				//файловый поток для записи
	
	//определение файловых потоков и выбор файла-источника
	switch (mode) {
	//прямой порядок копирования
	case 10:						//режим работы со списком товаров
	case 12:						//режим работы со списком поставщиков
	case -12:						//режим работы с таблицей движений
		dest = temp;				//файловый поток для записи
		source = fopen(src_file, "rb");//открыть файл для чтения
		
		//проверка наличия файла
		if (!source) {				//файл не найден
			return -2;				//вернуть код ошибки
		}//end if
		break;

	//обратный порядок копирования
	case 20:						//режим работы со списком товаров
	case 22:						//режим работы со списком поставщиков
	case -22:						//режим работы с таблицей движений
		
		source = temp;				//файловый поток для чтения
		
		dest = fopen(src_file, "wb");//очистить и открыть файл для записи
		
		//проверка наличия файла
		if (!dest) {				//файл не найден
			return -2;				//вернуть код ошибки
		}//end if
		break;
	}//end switch

	//расчет объема файла
	fseek(source, 0, SEEK_END);
	weight = ftell(source);
	rewind(source);					//вернуться в начало файла
	
	if (weight == 0) {				//файл пустой
		fclose(source);				//закрыть файл
		fclose(dest);				//закрыть файл
		return 0;					//завершение без ошибок
	}//end if
	
	switch (mode) {					//расчет длины строки файла
	//режим работы со списком товаров
	case 10:
	case 20:
		strweight = sizeof(int) * 4 + sizeof(char) * MAX_TOV;
		break;

	//режим работы со списком поставщиков
	case 12:
	case 22:
		strweight = sizeof(int) + sizeof(char) * MAX_POST;
		break;

	//режим работы с таблицей движений
	case -12:			
	case -22:
		strweight = sizeof(int) * 4 + sizeof(char) * 10;
		break;
	}//end switch

	while (ftell(source) < weight) {//пока файл не закончится
		product.tovID = 0;			//обнуление ID товара
		product.postID = 0;			//обнуление ID поставщика 
		
		//чтение товара из файла
		error = readProduct(source, 
			div(mode, 10).rem, product, ftell(source));
		if (error != 0) {			//произошла ошибка
			fclose(source);			//закрыть файл
			fclose(dest);			//закрыть файл
			return error;			//вернуть код ошибки
		}//end if
		
		//запись товара в файл
		error = pasteProduct(dest, div(mode, 10).rem, product);
		if (error != 0) {			//произошла ошибка
			fclose(source);			//закрыть файл
			fclose(dest);			//закрыть файл
			return error;			//вернуть код ошибки
		}//end if
	}//end while

	
	switch (mode) {					//закрыть открытый в подпрограмме файл
		//прямой порядок копирования
	case 10:						//режим работы со списком товаров
	case 12:						//режим работы со списком поставщиков
	case -12:						//режим работы с таблицей движений
		fclose(source);				//закрыть файл
		break;

		//обратный порядок копирования
	case 20:						//режим работы со списком товаров
	case 22:						//режим работы со списком поставщиков
	case -22:						//режим работы с таблицей движений
		fclose(dest);				//закрыть файл
		break;
	}//end switch

	return 0;						//завершение без ошибок

}//end cpyFile()

//проверки файла на корректное открытие и наличие данных
int checkFile(
	const int& mode, 				//режим работы
	const char(*src_file)) {		//имя файла-источника
	long weight;					//объем файла
	FILE* file;						//файловый поток для чтения
		
	file = fopen(src_file, "rb");	//открыть файл для чтения

	//проверка наличия файла
	if (!file) {					//файл не найден
		switch (mode) {
		case 1:						//необязательное наличие данных
			return -7;				//вернуть код ошибки

		case 2:						//результат записи данных 
		case 0:						//обязательное наличие данных
			return -2;				//вернуть код ошибки
		}//end switch
	}//end if

	//расчет объема файла
	fseek(file, 0, SEEK_END);
	weight = ftell(file);
	fclose(file);					//закрыть файл

	if (weight == 0) {				//файл пустой
		switch (mode) {
		case 1:						//необязательное наличие данных
			return -7;				//вернуть код ошибки
		
		case 2:						//результат записи данных
			return -6;				//вернуть код ошибки
		
		case 0:						//обязательное наличие данных
			return -5;				//вернуть код ошибки
		}//end switch
	}//end if
	
	return 0;						//завершение без ошибок

}//end checkFile()

//очистка файла
int cleanFile(
	const char(*src_file)) {		//имя файла-источника
	FILE* file;						//файловый поток для очистки

	file = fopen(src_file, "wb");	//очистить и открыть файл для записи

	//проверка наличия файла
	if (!file) {					//файл не найден
		return -2;					//вернуть код ошибки
	}//end if

	fclose(file);					//закрыть файл
	return 0;						//завершение без ошибок

}//end cleanFile()

//чтение товара из файла
int readProduct(
	FILE* base,						//файловый поток для чтения
	const int& mode, 				//режим работы
	struct part& product, 			//товар
	const long& place) {			//начало строки для чтения
	int error;						//код ошибки
	long weight;					//объем файла
	char string[LEN + 1] = "";		//буфер для чтения даты
	void* ptr;						//указатель на буфер для чтения
		
	//расчет объема файла
	fseek(base, 0, SEEK_END);
	weight = ftell(base);
		
	fseek(base, place, SEEK_SET);	//переход к строке с искомым товаром

	switch (mode) {
	case 0:							//режим работы со списком товаров
		//чтение ID товара из файла
		ptr = &product.tovID;
		error = fread(ptr, sizeof(int), 1, base);
		if (error < 1) {			//ошибка чтения из файла
			fclose(base);			//закрыть файл
			return -4;				//вернуть код ошибки
		}//end if

		//чтение названия товара из файла
		ptr = &string;
		error = fread(ptr, sizeof(char), MAX_TOV, base);
		if (error < (int)strlen(string)) {//ошибка чтения из файла
			fclose(base);			//закрыть файл
			return -4;				//вернуть код ошибки
		}//end if
		strncpy(product.tovName, "", MAX_TOV);
		sprintf(product.tovName, "%-*s", MAX_TOV, string);
		break;

	case 1:							//режим работы со счетом на закупку
	case -1:						//режим работы со счетом на продажу
	case -2:						//режим работы с таблицей движений
	case 3:							//режим работы с отчетом
		//чтение даты закупки/продажи товара из файла
		ptr = &string;
		error = fread(ptr, sizeof(char), 10, base);
		if (error < 10) {			//ошибка чтения из файла
			fclose(base);			//закрыть файл
			return -4;				//вернуть код ошибки
		}//end if

		//перевод даты из строки в структуру
		product.date = convertDate(string);
		break;
	}//end switch

	switch (mode) {
	case 0:							//режим работы со списком товаров
	case 1:							//режим работы со счетом на закупку
	case 2:							//режим работы со списком поставщиков
	case -2:						//режим работы с таблицей движений
	case 3:							//режим работы с отчетом
		//чтение ID поставщика из файла
		ptr = &product.postID;
		error = fread(ptr, sizeof(int), 1, base);
		if (error < 1) {			//ошибка чтения из файла
			fclose(base);			//закрыть файл
			return -4;				//вернуть код ошибки
		}//end if
		break;
	}//end switch

	switch (mode) {
	case 2:							//режим работы со списком поставщиков
		//чтение названия поставщика из файла
		ptr = &string;
		error = fread(ptr, sizeof(char), MAX_POST, base);
		if (error < (int)strlen(string)) {//ошибка чтения из файла
			fclose(base);			//закрыть файл
			return -4;				//вернуть код ошибки
		}//end if
		strncpy(product.postName, "", MAX_POST);
		sprintf(product.postName, "%-*s", MAX_POST, string);

		return 0;					//завершение без ошибок

	case -2:						//режим работы с таблицей движений
	case 1:							//режим работы со счетом на закупку
	case -1:						//режим работы со счетом на продажу
	case 3:							//режим работы с отчетом
		//чтение ID товара из файла
		ptr = &product.tovID;
		error = fread(ptr, sizeof(int), 1, base);
		if (error < 1) {			//ошибка чтения из файла
			fclose(base);			//закрыть файл
			return -4;				//вернуть код ошибки
		}//end if
		break;
	}//end switch

	//чтение количества товара из файла
	ptr = &product.count;
	error = fread(ptr, sizeof(int), 1, base);
	if (error < 1) {				//ошибка чтения из файла
		fclose(base);				//закрыть файл
		return -4;					//вернуть код ошибки
	}//end if

	//чтение цены товара из файла
	ptr = &product.cost;
	error = fread(ptr, sizeof(int), 1, base);
	if (error < 1) {				//ошибка чтения из файла
		fclose(base);				//закрыть файл
		return -4;					//вернуть код ошибки
	}//end if

	switch (mode) {
	case 1:							//режим работы со счетом на закупку
	case -1:						//режим работы со счетом на продажу
	case 3:							//режим работы с отчетом
		//чтение стоимости товара из файла
		ptr = &product.sum;
		error = fread(ptr, sizeof(int), 1, base);
		if (error < 1) {			//ошибка чтения из файла
			fclose(base);			//закрыть файл
			return -4;				//вернуть код ошибки
		}//end if
		break;
	}//end switch

	return 0;						//завершение без ошибок

}//end readProduct()

//запись товара в файл
int pasteProduct(
	FILE* base,						//файловый поток для записи
	const int& mode, 				//режим работы
	struct part& product) {			//товар
	int error;						//код ошибки
	char string[LEN + 1] = "";		//буфер для записи даты
	void* ptr;						//указатель на буфер для записи

	fseek(base, 0, SEEK_END);		//переход в конец файла

	switch (mode) {
	case 0:							//режим работы со списком товаров
		//запись ID товара в файл
		ptr = &product.tovID;
		error = fwrite(ptr, sizeof(int), 1, base);
		if (error < 1) {			//ошибка записи в файл
			fclose(base);			//закрыть файл
			return -3;				//вернуть код ошибки
		}//end if

		//запись названия товара в файл
		strncpy(string, "", LEN);
		sprintf(string, "%-*s", MAX_TOV, product.tovName);
		ptr = &string;
		error = fwrite(ptr, sizeof(char), strlen(string), base);
		if (error < (int)strlen(string)) {//ошибка записи в файл
			fclose(base);			//закрыть файл
			return -3;				//вернуть код ошибки
		}//end if
		break;

	case 1:							//режим работы со счетом на закупку
	case -1:						//режим работы со счетом на продажу
	case -2:						//режим работы с таблицей движений
	case 3:							//режим работы с отчетом
		//запись  даты закупки/продажи товара в файл
		strncpy(string, "", LEN);
		sprintf(string, "%0*d.%0*d.%0*d", 2, product.date.tm_mday, 
			2, product.date.tm_mon, 4, product.date.tm_year);
		ptr = &string;
		error = fwrite(ptr, sizeof(char), 10, base);
		if (error < 10) {			//ошибка записи в файл
			fclose(base);			//закрыть файл
			return -3;				//вернуть код ошибки
		}//end if
		break;
	}//end switch

	switch (mode) {
	case 0:							//режим работы со списком товаров
	case 1:							//режим работы со счетом на закупку
	case 2:							//режим работы со списком поставщиков
	case -2:						//режим работы с таблицей движений
	case 3:							//режим работы с отчетом
		//запись ID поставщика в файл
		ptr = &product.postID;
		error = fwrite(ptr, sizeof(int), 1, base);
		if (error < 1) {			//ошибка записи в файл
			fclose(base);			//закрыть файл
			return -3;				//вернуть код ошибки
		}//end if
		break;
	}//end switch

	switch (mode) {
	case 2:							//режим работы со списком поставщиков
		//запись названия поставщика в файл
		strncpy(string, "", LEN);
		sprintf(string, "%-*s", MAX_POST, product.postName);
		ptr = &string;
		error = fwrite(ptr, sizeof(char), strlen(string), base);
		if (error < (int)strlen(string)) {//ошибка записи в файл
			fclose(base);			//закрыть файл
			return -3;				//вернуть код ошибки
		}//end if

		return 0;					//завершение без ошибок

	case -2:						//режим работы с таблицей движений
	case 1:							//режим работы со счетом на закупку
	case -1:						//режим работы со счетом на продажу
	case 3:							//режим работы с отчетом
		//запись ID товара в файл
		ptr = &product.tovID;
		error = fwrite(ptr, sizeof(int), 1, base);
		if (error < 1) {			//ошибка записи в файл
			fclose(base);			//закрыть файл
			return -3;				//вернуть код ошибки
		}//end if
		break;
	}//end switch

	//запись количества товара в файл
	ptr = &product.count;
	error = fwrite(ptr, sizeof(int), 1, base);
	if (error < 1) {				//ошибка записи в файл
		fclose(base);				//закрыть файл
		return -3;					//вернуть код ошибки
	}//end if

	//запись цены товара в файл
	ptr = &product.cost;
	error = fwrite(ptr, sizeof(int), 1, base);
	if (error < 1) {				//ошибка записи в файл
		fclose(base);				//закрыть файл
		return -3;					//вернуть код ошибки
	}//end if

	switch (mode) {
	case 1:							//режим работы со счетом на закупку
	case -1:						//режим работы со счетом на продажу
	case 3:							//режим работы с отчетом
		//запись стоимости товара в файл
		product.sum = product.count * product.cost;
		ptr = &product.sum;
		error = fwrite(ptr, sizeof(int), 1, base);
		if (error < 1) {			//ошибка записи в файл
			fclose(base);			//закрыть файл
			return -3;				//вернуть код ошибки
		}//end if
		break;
	}//end switch

	return 0;						//завершение без ошибок

}//end pasteProduct()

/*=======================================================================/
/ Инструменты для печати данных                                          /
/=======================================================================*/

//печать списка товаров/поставщиков в консоль
int printList(
	const int& mode, 				//режим работы
	const int& postID) {			//ID поставщика
	int error;						//код ошибки
	long weight;					//объем файла
	long strweight;					//длина строки в байтах
	bool empty;						//флаг наличия напечатанных строк
	char s[LEN + 1] = "";			//технологическая строка для печати
	const char* source = NULL;		//указатель на имя файла-источника
	FILE* base;						//файловый поток для чтения
	struct part product;			//товар
	
	//выбор файла-источника и расчет длины строки файла
	switch (mode) {
	case 0:							//режим работы со списком товаров
		source = STORE;
		strweight = sizeof(int) * 3 + sizeof(char) * MAX_TOV;
		break;

	case 2:							//режим работы со списком поставщиков
		source = CONTRACTORS;
		strweight = sizeof(char) * MAX_POST;
		break;
	}//end switch
	
	base = fopen(source, "rb");		//открыть файл для чтения

	//проверка наличия файла
	if (!base) {
		return -2;					//файл не найден
	}//end if

	//расчет объема файла
	fseek(base, 0, SEEK_END);
	weight = ftell(base);
	rewind(base);					//вернуться в начало файла

	if (weight == 0) {				//файл пустой
		printf(eng("Данные не найдены.", s));
		printf(eng(" Доступен только ввод новых данных.\n\n", s));

		return 0;					//завершение без ошибок
	}//end if

	//инициализация флага наличия напечатанных строк
	empty = true;
		
	while (ftell(base) < weight) {	//пока файл не закончится
		//чтение товара из файла
		error = readProduct(base, mode, product, ftell(base));
		if (error != 0) {			//произошла ошибка
			fclose(base);			//закрыть файл
			return error;			//вернуть код ошибки
		}//end if

		//печать строки списка
		switch (mode) {
		case 0:						//режим работы со списком товаров
			//ID поставщика совпадает с искомым или фильтрация отсутствует
			if ((product.postID == postID) || (postID == 0)) {
				printf("%d\t%s\t%-5d %s\n", product.tovID,
					product.tovName, product.cost, eng("руб.", s));
				
				empty = false;		//хотя бы одна строка напечатана
			}//end if
			break;

		case 2:						//режим работы со списком поставщиков
			printf("%d\t%s\n", product.postID, product.postName);
				
			empty = false;			//хотя бы одна строка напечатана
			break;
		}//end switch
	}//end while
		
	if (empty) {					//не было напечатано ни одной строки
		printf(eng("Данные не найдены.", s));
		printf(eng(" Доступен только ввод новых данных.\n", s));
	}//end if
	printf("\n");

	return 0;						//завершение без ошибок

}//end printList()

//печать сообщения о результате работы программы
void printMessage(
	const int& error_code) {
	char s[LEN + 1] = "";			//технологическая строка для печати
		
	switch (error_code) {			//печать сообщения
	case 0:							//нормальное завершение программы
		printf(eng("\nОперация завершена успешно.\n\n", s));
		break;

	case -6:						//файл для записи пустой
		printf(eng("\nФайл не создан из-за недостатка данных.", s));

	case -1:						//операция отменена
		printf(eng("\nОперация отменена.\n\n", s));
		break;

	case -2:						//файл для чтения не найден
		printf(eng("\nФайл для чтения не найден.", s));
		printf(eng(" Операция прервана.\n\n", s));
		break;

	case -3:						//ошибка записи в файл
		printf(eng("\nВозникла ошибка записи файла.", s));
		printf(eng(" Операция прервана.\n\n", s));
		break;

	case -4:						//ошибка чтения из файла
		printf(eng("Возникла ошибка чтения файла.", s));
		printf(eng(" Операция прервана.\n", s));
		break;

	case -5:						//файл для чтения пустой
		printf(eng("\nФайл для чтения пустой.", s));
		printf(eng(" Операция прервана.\n\n", s));
		break;

	case -7:						//недостаточно данных для выполнения
		printf(eng("\nФайл с данными для чтения пустой. ", s));
		printf(eng("Операция недоступна.\n\n", s));
		break;
	}//end switch

}//end printMessage()

//печать содержимого файла в таблице
int printFile(
	const int& mode) {				//режим работы
	int error;						//код ошибки
	int sum;						//итоговая стоимость товаров
	int count;						//исходное количество товара
	long weight;					//объем файла
	const char* source = NULL;		//указатель на имя файла-источника
	FILE* base;						//файловый поток для чтения
	struct part product;			//товар
	
	switch (mode) {					//выбор файла-источника
	case 0:							//режим работы со списком товаров
		source = STORE;
		break;

	case 1:							//режим работы со счетом на закупку
		source = PURCHASE_INVOICE;
		break;

	case -1:						//режим работы со счетом на продажу
		source = SALE_INVOICE;
		break;

	case 2:							//режим работы со списком поставщиков
		source = CONTRACTORS;
		break;

	case -2:						//режим работы с таблицей движений
		source = TRAFFIC;
		break;

	case 3:							//режим работы с отчетом
		source = REPORT;
		break;
	}//end switch

	//проверка наличия данных в файле для чтения
	error = checkFile(1, source);
	if (error != 0) {				//произошла ошибка
		return error;				//вернуть код ошибки
	}//end if

	base = fopen(source, "rb");		//открыть файл для чтения
	printHeader(mode);				//печать шапки таблицы
	sum = 0;						//обнуление итоговой стоимости
	
	//расчет объема файла
	fseek(base, 0, SEEK_END);
	weight = ftell(base);
	rewind(base);					//вернуться в начало файла
		
	while (ftell(base) < weight) {	//пока файл не закончится
		//чтение товара из файла
		error = readProduct(base, mode, product, ftell(base));
		if (error != 0) {			//произошла ошибка
			fclose(base);			//закрыть файл
			return error;			//вернуть код ошибки
		}//end if

		//поиск названия товара и поставщика
		switch (mode) {
		case 2:						//режим работы со списком поставщиков
			break;

		case -1:					//режим работы со счетом на продажу
			count = product.count;	//сохранение исходного количества товара

			//поиск данных товара по ID
			error = fillProduct(product.tovID, product);
			if (error != 0) {		//произошла ошибка
				fclose(base);		//закрыть файл
				return error;		//вернуть код ошибки
			}//end if

			product.count = count;	//вставка исходного количества товара
			break;

		case -2:					//режим работы с таблицей движений
		case 1:						//режим работы со счетом на закупку
		case 3:						//режим работы с отчетом
			//поиск названия товара по ID
			error = findName(0, product.tovID, product.tovName);
			if (error != 0) {		//произошла ошибка
				fclose(base);		//закрыть файл
				return error;		//вернуть код ошибки
			}//end if

		case 0:						//режим работы со списком товаров
			//поиск названия поставщика по ID
			error = findName(2, product.postID, product.postName);
			if (error != 0) {		//произошла ошибка
				fclose(base);		//закрыть файл
				return error;		//вернуть код ошибки
			}//end if
			break;
		}//end switch
		
		sum += abs(product.sum);	//расчет итоговой стоимости товаров
		if (ftell(base) < weight) { //файл не закончился
			//печать строки файла с нижней границей
			printString(mode, product, true);
		}//end if	
	}//end while

	printString(mode, product, false);//печать последней строки файла
	printBorder(mode, sum);			//печать завершающей границы таблицы
	return 0;						//завершение без ошибок

}//end printFile()

//печать шапки таблицы
void printHeader(
	const int& mode) {				//режим работы
	int i, j;						//счетчики
	//символы для печати верхней границы шапки таблицы
	int chars1[4] = { 201, 205, 209, 187 };
	//ширина столбцов таблицы
	int length[8] = { 10, 4, 30, 4, 40, 6, 6, 11 };
	//символы для печати нижней границы шапки таблицы
	int chars2[5] = { 204, 205, 216, 185, 209 };
	char s[LEN + 1] = "";			//технологическая строка для печати

	//корректировка ширины столбцов таблицы
	switch (mode) {
	case 0:							//режим работы со списком товаров
		length[0] = 0;
		length[2] = 40;
		length[4] = 30;
		length[7] = 0;
		break;

	case -1:						//режим работы со счетом на продажу
		length[1] = 0;
		length[2] = 0;
		break;

	case 2:							//режим работы со списком поставщиков
		length[0] = 0;
		for (i = 3; i < 8; i++) {
			length[i] = 0;
		}//end for i
		break;

	case -2:						//режим работы с таблицей движений
		length[7] = 0;
		break;
	}//end switch

	//печать верхней границы шапки таблицы
	printf("%c", chars1[0]);
	for (i = 0; i < 8; i++) {
		if (length[i] != 0) {		//i-ый столбец печатается
			//печать границы столбца
			for (j = 0; j < length[i]; j++) {
				printf("%c", chars1[1]);
			}//end for j

			//выход, если i-ый столбец последний из печатаемых
			if (((i == 2) && (length[3] == 0)) ||
				((i == 6) && (length[7] == 0))) {
				break;
			}//end if

			//пропуск разделителя между столбцами "ID" и "название"
			if ((i == 1) || (i == 3)) {
				printf("%c", chars1[1]);
			} else if (i != 7) {	//i-ый столбец не последний
				printf("%c", chars1[2]);
			}//end if
		}//end if
	}//end for i
	printf("%c\n", chars1[3]);		//завершение строки

	//печать названий столбцов таблицы
	printf("%c", char(186));
	switch (mode) {
	case 1:							//режим работы со счетом на закупку
	case -1:						//режим работы со счетом на продажу
	case -2:						//режим работы с таблицей движений
	case 3:							//режим работы с отчетом
		printf("%s%c", eng("   Дата   ", s), char(179));
		break;
	}//end switch

	switch (mode) {
	case 1:							//режим работы со счетом на закупку
	case -2:						//режим работы с таблицей движений
	case 3:							//режим работы с отчетом
	case 2:							//режим работы со списком поставщиков
		printf("%-*s", MAX_POST + length[1] + 1,
			eng("ID и название поставщика", s));
		break;
	}//end switch

	switch (mode) {
	case 2:							//режим работы со списком поставщиков
		printf("%c\n", char(186));	//завершение строки
		break;

	case 1:							//режим работы со счетом на закупку
	case -2:						//режим работы с таблицей движений
	case 3:							//режим работы с отчетом
		printf("%c", char(179));	//завершение строки

	default:						//остальные режимы
		printf("%-*s%c", MAX_TOV + length[3] + 1,
			eng("ID и название товара", s), char(179));
		break;
	}//end switch

	switch (mode) {
	case 2:							//режим работы со списком поставщиков
		break;

	case 0:							//режим работы со списком товаров
		printf("%-*s%c", MAX_POST + length[1] + 1,
			eng("ID и название поставщика", s), char(179));

	default:						//остальные режимы
		printf("%s%c", eng("Кол-во", s), char(179));
		printf(eng(" Цена ", s));
		break;
	}//end switch

	switch (mode) {
	case 0:							//режим работы со списком товаров
	case -2:						//режим работы с таблицей движений
		//завершение строки
		printf("%c\n", char(186));
		break;

	case 1:							//режим работы со счетом на закупку
	case -1:						//режим работы со счетом на продажу
	case 3:							//режим работы с отчетом
		printf("%c%s%c\n", char(179), eng(" Стоимость ", s), char(186));
		break;
	}//end switch
	
	//печать нижней границы шапки таблицы
	printf("%c", chars2[0]);
	for (i = 0; i < 8; i++) {
		if (length[i] != 0) {		//i-ый столбец печатается
			//печать границы столбца
			for (j = 0; j < length[i]; j++) {
				printf("%c", chars2[1]);
			}//end for j

			//выход, если i-ый столбец последний из печатаемых
			if (((i == 2) && (length[3] == 0)) ||
				((i == 6) && (length[7] == 0))) {
				break;
			}//end if

			//пропуск разделителя между столбцами "ID" и "название"
			if ((i == 1) || (i == 3)) {
				printf("%c", chars2[4]);
			} else if (i != 7) {	//i-ый столбец не последний
				printf("%c", chars2[2]);
			}//end if
		}//end if
	}//end for i
	printf("%c\n", chars2[3]);		//завершение строки

}//end printHeader()

//печать строки таблицы
void printString(
	const int& mode, 				//режим работы
	struct part& product,			//товар
	const bool& flag) {				//флаг печати нижней границы
	int i, j;						//счетчики
	//ширина столбцов таблицы
	int length[8] = { 10, 4, 30, 4, 40, 6, 6, 11 };
	//символы для печати нижней границы строки таблицы
	int chars[4] = { 199, 196, 197, 182 };
	char s[LEN + 1] = "";			//технологическая строка для печати

	//корректировка ширины столбцов таблицы
	switch (mode) {
	case 0:							//режим работы со списком товаров
		length[0] = 0;
		length[2] = 40;
		length[4] = 30;
		length[7] = 0;
		break;

	case -1:						//режим работы со счетом на продажу
		length[1] = 0;
		length[2] = 0;
		break;

	case 2:							//режим работы со списком поставщиков
		length[0] = 0;
		for (i = 3; i < 8; i++) {
			length[i] = 0;
		}//end for i
		break;

	case -2:						//режим работы с таблицей движений
		length[7] = 0;
		break;
	}//end switch

	//печать данных в строке таблицы
	printf("%c", char(186));
	switch (mode) {
	case 3:							//режим работы с отчетом
	case -1:						//режим работы со счетом на продажу
		//удаление знака количества товара для печати счетов и отчетов
		product.count = abs(product.count);
		product.sum = abs(product.sum);

	case 1:							//режим работы со счетом на закупку
	case -2:						//режим работы с таблицей движений
		//печать даты закупки/продажи
		printf("%0*d.%0*d.%0*d%c", 2, product.date.tm_mday, 2,
			product.date.tm_mon, 4, product.date.tm_year, char(179));
		break;
	}//end switch

	switch (mode) {
	case 1:							//режим работы со счетом на закупку
	case -2:						//режим работы с таблицей движений
	case 3:							//режим работы с отчетом
	case 2:							//режим работы со списком поставщиков
		//печать ID и названия поставщика
		printf("%0*d%c", length[1], product.postID, char(179));
		printf("%-*s", MAX_POST, product.postName);
		break;
	}//end switch

	switch (mode) {
	case 2:							//режим работы со списком поставщиков
		printf("%c\n", char(186));	//завершение строки
		break;

	case 1:							//режим работы со счетом на закупку
	case -2:						//режим работы с таблицей движений
	case 3:							//режим работы с отчетом
		printf("%c", char(179));	//завершение строки

	default:						//остальные режимы
		//печать ID и названия поставщика
		printf("%0*d%c", length[3], product.tovID, char(179));
		printf("%-*s%c", MAX_TOV, product.tovName, char(179));
		break;
	}//end switch

	switch (mode) {
	case 2:							//режим работы со списком поставщиков
		break;

	case 0:							//режим работы со списком товаров
		//печать ID и названия поставщика
		printf("%0*d%c", length[1], product.postID, char(179));
		printf("%-*s%c", MAX_POST, product.postName, char(179));

	default:						//остальные режимы
		//печать количества товара
		printf("%-*d%c", length[5], product.count, char(179));

		//печать цены товара
		printf("%-*d", length[6], product.cost);
		break;
	}//end switch

	switch (mode) {
	case -2:						//режим работы с таблицей движений
	case 0:							//режим работы со списком товаров
		printf("%c\n", char(186));	//завершение строки
		break;

	case 1:							//режим работы со счетом на закупку
	case -1:						//режим работы со счетом на продажу
	case 3:							//режим работы с отчетом
		//печать стоимости товара
		printf("%c%-7d%s%c\n", char(179), 
			product.sum, eng("руб.", s), char(186));
		break;
	}//end switch

	//печать нижней границы строки, если она не последняя
	if (flag) {
		printf("%c", chars[0]);
		for (i = 0; i < 8; i++) {
			if (length[i] != 0) {	//i-ый столбец печатается
				//печать границы столбца
				for (j = 0; j < length[i]; j++) {
					printf("%c", chars[1]);
				}//end for j

				//выход, если i-ый столбец последний из печатаемых
				if (((i == 2) && (length[3] == 0)) ||
					((i == 6) && (length[7] == 0))) {
					break;
				}//end if

				if (i != 7) {		//i-ый столбец не последний
					printf("%c", chars[2]);
				}//end if
			}//end if
		}//end for i
		printf("%c\n", chars[3]);	//завершение строки
	}//end if

}//end printString()

//печать нижней границы таблицы
void printBorder(
	const int& mode, 				//режим работы
	const int& sum) {				//итоговая сумма товаров
	int i, j;						//счетчики
	int leng = 0;					//длина строки таблицы
	//ширина столбцов таблицы
	int length[8] = { 10, 4, 30, 4, 40, 6, 6, 11 };
	//символы для печати нижней границы таблицы
	int chars[4] = { 200, 205, 207, 188 };
	//символы для печати строки итоговой стоимости
	int chars1[5] = { 204, 205, 207, 216, 185 };
	char s[LEN + 1] = "";			//технологическая строка для печати

	//корректировка ширины столбцов и расчет длины строки таблицы
	switch (mode) {
	case 0:							//режим работы со списком товаров
		length[0] = 0;
		length[2] = 40;
		length[4] = 30;
		length[7] = 0;
		break;

	case -1:						//режим работы со счетом на продажу
		length[1] = 0;
		length[2] = 0;
		leng = 70;
		break;

	case 2:							//режим работы со списком поставщиков
		length[0] = 0;
		for (i = 3; i < 8; i++) {
			length[i] = 0;
		}//end for i
		break;

	case -2:						//режим работы с таблицей движений
		length[7] = 0;
		break;

	case 1:							//режим работы со счетом на закупку
	case 3:							//режим работы с отчетом
		leng = 106;
	}//end switch

	//печать строки итоговой стоимости
	switch (mode) {
	case 1:							//режим работы со счетом на закупку
	case -1:						//режим работы со счетом на продажу
	case 3:							//режим работы с отчетом
		//печать верхней границы строки
		printf("%c", chars1[0]);
		for (i = 0; i < 8; i++) {
			if (length[i] != 0) {	//i-ый столбец печатается
				//печать границы столбца
				for (j = 0; j < length[i]; j++) {
					printf("%c", chars1[1]);
				}//end for j
				//печать разделителя перед столбцом итоговой стоимости
				if (i == 6) {
					printf("%c", chars1[3]);
				} else if (i != 7) {//i-ый столбец не последний
					printf("%c", chars1[2]);
				}//end if
			}//end if
		}//end for i
		printf("%c\n", chars1[4]);	//завершение строки

		//печать строки итоговой стоимости товаров
		printf("%c%*s%c", char(186), leng, eng("ИТОГО:", s), char(179));
		printf("%-9d%s%c\n", sum, eng("р.", s), char(186));
	}//end switch		

	//печать нижней границы таблицы
	printf("%c", chars[0]);
	for (i = 0; i < 8; i++) {
		if (length[i] != 0) {		//i - ый столбец печатается
			//печать границы столбца
			for (j = 0; j < length[i]; j++) {
				printf("%c", chars[1]);
			}//end for j

			//печать разделителей
			switch (mode) {
			case 1:					//режим работы со счетом на закупку
			case -1:				//режим работы со счетом на продажу
			case 3:					//режим работы с отчетом
				//печать разделителя перед столбцом итоговой стоимости
				if (i == 6) {
					printf("%c", chars1[2]);
				} else if (i != 7) {	//i-ый столбец не последний
					printf("%c", chars1[1]);
				}//end if
				break;

			case 2:					//режим работы со списком поставщиков
			case -2:				//режим работы с таблицей движений
			case 0:					//режим работы со списком товаров
				//выход, если i-ый столбец последний из печатаемых
				if (((i == 2) && (length[3] == 0)) ||
					((i == 6) && (length[7] == 0))) {
					break;
				}//end if
				if (i != 7) {		//i-ый столбец не последний
					printf("%c", chars[2]);
				}//end if
				break;
			}//end switch
		}//end if
	}//end for i
	printf("%c\n", chars[3]);		//завершение строки

}//end printBorder()
