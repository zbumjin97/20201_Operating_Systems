# 20201_Operating_Systems
2020학년도 1학기 운영체제


---
### OS Multithread
#### Thread의 병렬적 수행
* 10개의 Thread가 서로 공평하게 CPU를 할당받게 하도록 mutex의 lock과 unlock을 통해 Context switching 구현
* Thread 0과 Thread 1~9를 서로 분리하여, Thread 0이 가장 먼저 수행하는 함수는 그대로 두고, 나머지 Thread는 수행 이전에 또 다른 Thread로부터 signal을 받아서 mutex를 unlock해야만 수행되도록 pthread 라이브러리 함수를 조절
#### 결과값 저장
* 1차원 배열 int number[100]은 10개의 Thread가 모두 공유하는 변수로 지정하고 초깃값이 모두 0인 배열을 Thread에서 사용하였고, 따라서 결과값을 저장하기 위한 2차원 배열을 따로 만들고 포인터를 적절히 활용하여 n행의 결과는 n번째 Thread에서 계산한 결과값을 저장하도록 구현
#### 함수 인자 및 반환값
* pthread_create 및 pthread_join에서 사용하는 인자의 자료형을 int* type로 설정하여 int형 변수를 인자로 사용
* 반환값을 배열로 설정하였을 때 pthread_join에서 값을 받아오고 결과값을 따로 저장하는 공유 변수인 2차원 배열은 number[100] 배열을 통해 return value 없이 결과 출력


---
### 병렬 파일 저장
#### 이미지 파일 열기
* main 함수에서 xps.jpg 와 scenery.jpg binary file은 파일을 읽을 때 “rb” (read binary file) 모드로 읽음
* 나누어질 파일의 크기가 비슷하게 나누어지도록 이미지 파일의 file pointer (file descriptor)을 파일의 끝에 위치한 이후 fseek 함수를 통해 파일의 크기를 구함
* filelist라는 이차원 배열은 original file의 메타데이터를 저장하며, 각 행의 첫 번째 열은 original file이 분할될 파일의 개수를 저장하며, 두 번째 열부터는 original file의 이름을 저장함
#### 파일 나누기
* file_division 함수에서, 나누어질 파일의 이름을 지정하기 위해 original file의 이름을 함수의 매개 변수로 사용한 이후, split file의 번호와 문자열을 생성하여 파일 이름을 만듦
* 나누어질 파일의 개수만큼 split file을 생성하고 original file의 크기를 파일의 개수로 나눈 값만큼 split file의 크기로 지정한 다음, file pointer을 이동시키면서 original file을 split files로 나누어서 저장
* xps.jpg는 6개의 파일로 분할하고, scenery.jpg는 8개의 파일로 분할 (n1 = 6, n2 = 8)
* 나누어진 파일의 이름은 para_xps0.bin ~ para_xps5.bin 과 para_scenery0.bin ~ para_scenery7.bin이며, original file이 저장된 디렉토리 안에 함께 저장됨
#### 파일 병합하기
* file_combination 함수에서, 병합하여 생성될 파일을 binary write 모드로 열고 모든 file의 이름을 문자열로 생성
* filename이 예시로 “para_xps0.jpg”가 되게 하기 위하여 original file의 이름과 문자열 “para_”, 나누어진 파일 번호인 i, 확장자 “.jpg”를 sprintf 및 strcat로 만듦
* 나누어진 파일의 크기를 fseek 함수를 통해 구하였고, 파일의 크기만큼 반복문을 이용하여 나누어진 파일로부터 restore된 파일로 write하도록 구현
* 병합할 파일의 이름은 “re_xps.jpg”와 “re.scenery.jpg”로 생성되며, original file의 이미지와 완전히 같은 파일 크기와 이미지를 가지는 파일이 만들어짐
