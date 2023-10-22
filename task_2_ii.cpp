/*
    Алгоритм:
    1) Задать количество кластеров (множеств)
    2) Выбрать начальные центры случайным образом
    3) Посчитать расстояние от каждого центра до каждого пикселя
     3.1) Пересчитать все центры
    4) Распределить пиксели по множествам (по минимальному расстоянию до центра)
    5) Повторять 3-4п. пока пиксели не будут оставаться в одних и тех же множествах (+ ограничить число ит-ий цикла)

    Массив компонент:
     0 - Синий цвет
     1 - Зелёный цвет
     2 - Красный цвет
*/
#include <iostream>
#include <Windows.h>
#include <math.h>
#include <list>
#include <vector>
#include <algorithm>
#include <iterator>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

const int count_klaster = 8; // Число множеств (кластеров)
const int max_iter = 150; // Максимальное число итераций, если пиксели будут на границах множеств

struct rgb
{
    double r;
    double g;
    double b;
};

vector<rgb> set_pixels(string filename); // Функция получения изображения и заполнения вектора пикселей
vector<rgb> set_starting_center(vector<rgb> pixels); // Функция для получения начальных центров в случайной позиции
double calc_distance(rgb d, rgb c);// Функция расчета расстояния между пикселями
rgb recalc_center(vector<rgb> pixels, vector<int> check_1, int i);// Функция перерасчета центров

int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    setlocale(LC_ALL, "rus");

    string filename; // Переменная, хранящая имя исходного файла (*.jpg)
    vector<rgb> pixels; // Вектор, хранящий rgb-информацию о всех пикселях исходного файла
    vector<rgb> new_pixels; // Вектор для измененных пикселей, подогнанных под центры
    vector<rgb> centers; // Вектор, хранящий rgb-информацию о всех центрах
    vector<rgb>::iterator it;
    rgb grad_arr[count_klaster]; // Массив для построения градуировочных полос
    int min_it = 0; // Переменная, хранящая индекс центра, расстояние до которого минимально
    double arr[count_klaster]; // Массив для хранения расстояний до центров

    cout << "Введите название файла" << endl << "C:/Users/Sadness/Desktop/foto_for_ii/";
    cin >> filename;
    pixels = set_pixels(filename); // заполняем вектор пикселей
    new_pixels = pixels;
    centers = set_starting_center(pixels); // назначаем начальные центры
    for (int i = 0; i < centers.size(); i++)
        cout << "Начальный центр №" << i << "(r): " << centers[i].r << endl << "Начальный центр №" << i << "(g): " << centers[i].g << endl << "Начальный центр №" << i << "(b): " << centers[i].b << endl << endl;

    /*=== Основной алгоритм ===*/
    vector<int> check_1(pixels.size(), -1); // первый вектор, хранящий индексы центров, ближайших ко всем пикселям (при инициализации заполняется значениями -1)
    vector<int> check_2(pixels.size(), -2); // второй вектор, хранящий индексы центров, ближайших ко всем пикселям для следующей итерации (при инициализации заполняется значениями -2)
    int iter = 0; // для подсчета итераций

    while (iter < max_iter)
    {
        /*=== Определяем минимальное расстояние для каждого пикселя ===*/
        for (int i = 0; i < pixels.size(); i++) // прогоняем все пиксели
        {
            for (int j = 0; j < count_klaster; j++) // берем пиксель и проверяем его расстояние до каждого центра
            {
                arr[j] = calc_distance(pixels[i], centers[j]); // считаем расстояние до центра и заносим в массив
            }

            double min_dist = arr[0]; // переменная для хранения минимального расстояния, значение инициализации - первый элемент массива arr
            int min_it = 0; // переменная для хранения индекса минимального расстояния
            for (int j = 0; j < count_klaster; j++) // ищем минимальное расстояние
            {
                if (arr[j] < min_dist)
                {
                    min_dist = arr[j];
                    min_it = j;
                }
            }
            check_1[i] = min_it; // индекс центра, ближайшего к пикселю i заносим в вектор check_1
        }

        /*=== Пересчитываем центры ===*/
        for (int i = 0; i < count_klaster; i++) // прогоняем все центры
        {
            centers[i].r = recalc_center(pixels, check_1, i).r;
            centers[i].g = recalc_center(pixels, check_1, i).g;
            centers[i].b = recalc_center(pixels, check_1, i).b;
        }

        if (check_1 == check_2)
            break;

        check_2 = check_1;
        iter++;
    }

    cout << "iter = " << iter << endl << endl;
    cout << "Конечные центры:" << endl;
    for (int i = 0; i < centers.size(); i++)
        cout << "Центр №" << i << " (r): " << floor(centers[i].r) << endl << "Центр №" << i << " (g): " << floor(centers[i].g) << endl << "Центр №" << i << " (b): " << floor(centers[i].b) << endl << endl;

    /*=== Выводим изначальное и полученное изображения, а также полосы цветов множеств ===*/
    Mat in_img = imread("C:/Users/Sadness/Desktop/foto_for_ii/" + filename);
    Mat out_img = imread("C:/Users/Sadness/Desktop/foto_for_ii/" + filename);
    Mat grad = imread("C:/Users/Sadness/Desktop/foto_for_ii/grad.jpg");

    for (int i = 0; i < pixels.size(); i++) // Меняем все пиксели на центры, которым они принадлежат
    {
        new_pixels[i].r = floor(centers[check_2[i]].r);
        new_pixels[i].g = floor(centers[check_2[i]].g);
        new_pixels[i].b = floor(centers[check_2[i]].b);
    }
    reverse(new_pixels.begin(), new_pixels.end()); // Переворачиваем вектор, чтобы нормально вывести его

    for (int i = 0; i < centers.size(); i++)
    {
        grad_arr[i].r = centers[i].r;
        grad_arr[i].g = centers[i].g;
        grad_arr[i].b = centers[i].b;
    }

    int p_grad_count = 50;
    int cnt = 0;
    for (int i = 0; i < grad.rows; i++) // циклы для заполнения градуировочных полос
    {
        for (int j = 0; j < grad.cols; j++)
        {
            if (j < p_grad_count)
            {
                grad.at<Vec3b>(i, j)[0] = grad_arr[cnt].b;
                grad.at<Vec3b>(i, j)[1] = grad_arr[cnt].g;
                grad.at<Vec3b>(i, j)[2] = grad_arr[cnt].r;
            }
            else
            {
                p_grad_count += 50;
                j--;
                cnt++;
            }
            if ((p_grad_count - 50) / count_klaster == 50)
                break;
        }
        cnt = 0;
        p_grad_count = 50;
    }

    for (int i = 0; i < out_img.rows; i++) // циклы заполнения полученного после обрабтки изображения
        for (int j = 0; j < out_img.cols; j++)
        {
            out_img.at<Vec3b>(i, j)[0] = new_pixels.back().b;
            out_img.at<Vec3b>(i, j)[1] = new_pixels.back().g;
            out_img.at<Vec3b>(i, j)[2] = new_pixels.back().r;
            new_pixels.pop_back();
        }

    // Вывод всех изображений
    imshow("input_image", in_img);
    imshow("output_image", out_img);
    imshow("grad", grad);
    waitKey(0);

    // Запись обработанного изображения и градуировочных полос в файлы
    imwrite("C:/Users/Sadness/Desktop/foto_for_ii/output_image.jpg", out_img);
    imwrite("C:/Users/Sadness/Desktop/foto_for_ii/grad_color_image.jpg", grad);

    return 0;
}

vector<rgb> set_pixels(string filename)
{
    Mat img = imread("C:/Users/Sadness/Desktop/foto_for_ii/" + filename);
    rgb dot = {0, 0, 0};
    vector<rgb> pixels;

    for (int i = 0; i < img.rows; i++)
        for (int j = 0; j < img.cols; j++)
        {
            dot.b = img.at<Vec3b>(i, j)[0];
            dot.g = img.at<Vec3b>(i, j)[1];
            dot.r = img.at<Vec3b>(i, j)[2];
            pixels.push_back(dot);
        }
    return pixels;
}

vector<rgb> set_starting_center(vector<rgb> pixels)
{
    srand((unsigned)time(NULL));
    vector<rgb> starting_center;
    rgb dot = {0, 0, 0};

    for (int i = 0; i < count_klaster; i++) // Цикл с числом итераций = количеству множеств => количеству центров
    {
        dot = pixels[0 + rand() % pixels.size()]; // Выбираем случайный пиксель из всех имеющихся
        if (starting_center.empty())
            starting_center.push_back(dot); // Заносим новый первоначальный центр в вектор
        else
        {
            for (int j = 0, c = starting_center.size(); j < c; j++) // Проходимся по всем тем центрам, которые уже выбрали
            {
                if ((dot.r == starting_center[j].r) && (dot.g == starting_center[j].g) && (dot.b == starting_center[j].b)) // Если есть совпадения
                {
                    i--; // откатываем итерацию
                    break;
                }
                else
                {
                    if (j == starting_center.size() - 1)
                        starting_center.push_back(dot); // Заносим новый первоначальный центр в вектор
                }
            }
        }
    }

    return starting_center;
}

double calc_distance(rgb d, rgb c)
{
    return sqrt(pow((d.r - c.r), 2) + pow((d.g - c.g), 2) + pow((d.b - c.b), 2));
}

rgb recalc_center(vector<rgb> pixels, vector<int> check_1, int i)
{
    rgb sum = { 0, 0, 0 }; // Переменная для хранения суммы компонент всех пикселей определенного множества
    int count = 0; // Переменная для подсчета количества пикселей, относящихся к определенному множеству
    for (int j = 0; j < pixels.size(); j++)
    {
        if (check_1[j] == i) // Если пиксель принадлежит центру, который в данный момент пересчитывается
        {
            sum.r += pixels[j].r; // суммируем его компоненты
            sum.g += pixels[j].g;
            sum.b += pixels[j].b;
            count++; // считаем сколько пикселей принадлежит этому центру
        }
    }
    sum.r = sum.r / count; // и делим на количество пикселей => получаем среднее арифметическое
    sum.g = sum.g / count;
    sum.b = sum.b / count;

    return sum;
}