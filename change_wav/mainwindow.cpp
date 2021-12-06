#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QFileDialog>
#include<QMessageBox>
#include <iostream>
#include <cstdio>
#include <cmath>
#include "string.h"
#include "mem.h"
#include<QDataStream>

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{

    delete ui;
}

//Wav Header
struct wav_header_t
{
    char chunkID[4]; //"RIFF" = 0x46464952
    unsigned long chunkSize; //28 [+ sizeof(wExtraFormatByt es) + wExtraFormatBytes] + sum(sizeof(chunk.id) + sizeof(chunk.size) + chunk.size)
    char format[4]; //"WAVE" = 0x45564157
    char subchunk1ID[4]; //"fmt " = 0x20746D66
    unsigned long subchunk1Size; //16 [+ sizeof(wExtraFormatBytes) + wExtraFormatBytes]
    unsigned short audioFormat;
    unsigned short numChannels;
    unsigned long sampleRate;
    unsigned long byteRate;
    unsigned short blockAlign;
    unsigned short bitsPerSample;
    //[WORD wExtraFormatBytes;]
    //[Extra format bytes]
};

//Chunks
struct chunk_t
{
    char ID[4]; //"data" = 0x61746164
    unsigned long size;  //Chunk data bytes
};

struct struct_set
{
    struct wav_header_t header_set;
    struct chunk_t chunk_set;
};

struct_set WavReader(const char* fileName, const char* fileToSave1, const char* fileToSave2)
{
    struct_set hihi;
    FILE *fin = fopen(fileName, "rb");

    //Read WAV header
    wav_header_t header;
    fread(&header, sizeof(header), 1, fin);

    //Print WAV header
    printf("WAV File Header read:\n");
    printf("File Type: %s\n", header.chunkID);
    printf("File Size: %ld\n", header.chunkSize);
    printf("WAV Marker: %s\n", header.format);
    printf("Format Name: %s\n", header.subchunk1ID);
    printf("Format Length: %ld\n", header.subchunk1Size );
    printf("Format Type: %hd\n", header.audioFormat);
    printf("Number of Channels: %hd\n", header.numChannels);
    printf("Sample Rate: %ld\n", header.sampleRate);
    printf("Sample Rate * Bits/Sample * Channels / 8: %ld\n", header.byteRate);
    printf("Bits per Sample * Channels / 8.1: %hd\n", header.blockAlign);
    printf("Bits per Sample: %hd\n", header.bitsPerSample);

    //skip wExtraFormatBytes & extra format bytes
    //fseek(f, header.chunkSize - 16, SEEK_CUR);

    //Reading file
    chunk_t chunk;
    printf("id\t" "size\n");
    //go to data chunk
    while (true)
    {
        fread(&chunk, sizeof(chunk), 1, fin);
        printf("%c%c%c%c\t" "%li\n", chunk.ID[0], chunk.ID[1], chunk.ID[2], chunk.ID[3], chunk.size);
        if (*(unsigned int *)&chunk.ID == 0x61746164)
            break;
        //seskip chunk data byt
        fseek(fin, chunk.size, SEEK_CUR);
    }

    //Number of samples
    int sample_size = header.bitsPerSample / 8;
    int samples_count = chunk.size * 8 / header.bitsPerSample;
    printf("Samples count = %i\n", samples_count);

    short int *value = new short int[samples_count];
    memset(value, 0, sizeof(short int) * samples_count);

    //Reading data
    for (int i = 0; i < samples_count; i++)
    {
        fread(&value[i], sample_size, 1, fin);
    }

    //Write data into the file
    FILE *fout1 = fopen(fileToSave1, "w"); //write 쓰기용
    FILE *fout2 = fopen(fileToSave2, "w"); //write 쓰기용
    for (int i = 0; i < samples_count; i++)
    {
        if(i%2==0)
            fprintf(fout1, "%d\n", value[i]);//value값을 파일아웃으로 쓰기
        else
            fprintf(fout2, "%d\n", value[i]);

    }
    fclose(fin);
    fclose(fout1);
    fclose(fout2);

    hihi.header_set = header;
    hihi.chunk_set = chunk;

    return hihi;

}


void MainWindow::on_pushButton_clicked()
{
    QString OpenFile=QFileDialog::getOpenFileName(
                this,
                tr("Open File"),
                "C://",
                "All files (*.*);;Text File (*.txt);;wav file(*.wav)"
                );

    QMessageBox::information(this,tr("Open File Path"),OpenFile);

//if문 사용으로 channel control

    QString SaveFile1=QFileDialog::getSaveFileName(
                this,
                tr("Save File 1"),
                "C://",
                "All files (*.*);;Text File (*.txt);;dat file(*.dat)"
                );

    QMessageBox::information(this,tr("Save File 1 Path"),SaveFile1);

    QString SaveFile2=QFileDialog::getSaveFileName(
                this,
                tr("Save File 2"),
                "C://",
                "All files (*.*);;Text File (*.txt);;dat file(*.dat)"
                );

    QMessageBox::information(this,tr("Save File 2 Path"),SaveFile2);

    char open_str[1024] = {0,};
    qsnprintf(open_str, sizeof(open_str), "%s", OpenFile.toUtf8().constData());

    char save1_str[1024] = {0,};
    qsnprintf(save1_str, sizeof(save1_str), "%s", SaveFile1.toUtf8().constData());

    char save2_str[1024] = {0,};
    qsnprintf(save2_str, sizeof(save2_str), "%s", SaveFile2.toUtf8().constData());


    struct_set test = WavReader(open_str, save1_str, save2_str);


    QString input_chunkID = QString(test.header_set.chunkID);
    ui->lineEdit->setText(input_chunkID);


    QString input_chunkSize;
    input_chunkSize.setNum(test.header_set.chunkSize);
    ui->lineEdit_2->setText(input_chunkSize);


    QString input_wavmaker = QString(test.header_set.format);
    ui->lineEdit_3->setText(input_wavmaker);


    QString input_formatname = QString(test.header_set.subchunk1ID);
    ui->lineEdit_4->setText(input_formatname);


    QString input_formatLength = QString::number(test.header_set.subchunk1Size);
    ui->lineEdit_5->setText(input_formatLength);


    QString input_formatType = QString::number(test.header_set.audioFormat);
    ui->lineEdit_6->setText(input_formatType);


    QString input_numchannels = QString::number(test.header_set.numChannels);
    ui->lineEdit_7->setText(input_numchannels);

    QString input_sampleRate = QString::number(test.header_set.sampleRate);
    ui->lineEdit_8->setText(input_sampleRate);


    QString input_sampleRatebits = QString::number(test.header_set.byteRate);
    ui->lineEdit_11->setText(input_sampleRatebits);


    QString input_bitsperSamplechannels = QString::number(test.header_set.blockAlign);
    ui->lineEdit_10->setText(input_bitsperSamplechannels);

    QString input_bitsperSample = QString::number(test.header_set.bitsPerSample);
    ui->lineEdit_9->setText(input_bitsperSample);

    QString input_data = QString::number(test.chunk_set.size);
    ui->lineEdit_12->setText(input_data);

    QString input_samplescount = QString::number(test.chunk_set.size * 8 / test.header_set.bitsPerSample);
    ui->lineEdit_13->setText(input_samplescount);

}
