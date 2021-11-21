#include <string>
#include <vector>
#include <sstream>
#include <string.h>
#include <stdio.h>

//hexToBinで使う
int ch2num(char ch1,char ch2)
{
	int nout;
	if((ch1 >= '0')&&('9' >= ch1)) nout = (ch1 & 0xf) * 0x10;
	else nout = ((ch1 & 0xf) + 9) * 0x10;
	if((ch2 >= '0')&&('9' >= ch2)) return (ch2 & 0xf) + nout;
	else return ((ch2 & 0xf) + 9) + nout;
}


void split(  
    const std::string& str,   
    const std::string& delim,   
    std::vector<std::string>& result)  
{  
    std::string::size_type offset = 0;  
    std::string::size_type pos = 0;  
    std::string::const_iterator begin = str.begin();  
    std::string::const_iterator end = str.end();  
  
    while((pos = str.find_first_of(delim, offset)) != str.npos)  
    {  
        if(offset != pos)  
        {  
            result.push_back(std::string(begin + offset, begin + pos));  
        }  
        offset = pos + 1;  
    }  
    result.push_back(std::string(begin + offset, end));  
}  


//Intel-Hex形式をバイト配列に変換する
//返値は、使用するアドレスのうちもっとも大きいもの+1
long HexToBin(std::string &hex, unsigned char* bin, int maxbinsize)	//static
{
	int lsize,dmode;
	char lbuf[256];
	long po,adrs,ofsa,abadrs,maxadr=-1;
	std::vector<std::string> lines;
	int i;

	//split
	split(hex, "\n", lines);
/*
	//[debug]
	for(i=0; i<(int)lines.size(); i++){	
		printf("%s\n", lines[i].c_str());		
	}
*/
	//parse
	adrs = 0; ofsa = 0;
	for(i=0; i<(int)lines.size(); i++){	/*Loop until end of data*/
		strcpy(lbuf, lines[i].c_str());

		if((lbuf[0] != ':')||(!isalnum(lbuf[1]))||(!isalnum(lbuf[2]))) {
    		continue;
		}
		lsize = ch2num(lbuf[1],lbuf[2]) * 2;
		adrs = (long)(ch2num(lbuf[3],lbuf[4])) * 0x100L 
			+ (long)(ch2num(lbuf[5],lbuf[6]));
		if(maxbinsize < (ofsa + adrs)) {/*Address check*/
			return 0;
		}

		dmode = ch2num(lbuf[7],lbuf[8]);/*Get data mode */
		if(dmode == 2) {/*Get offset address*/
    		ofsa = ((long)ch2num(lbuf[9],lbuf[10]) * 0x100L
			+ (long)ch2num(lbuf[11],lbuf[12])) * 0x10L;
			continue;
		}
		if(dmode == 0){/*Get binary data*/
			abadrs = ofsa + adrs;
			for(po = 9;po < (lsize+9);po=po+2){
				if(abadrs>maxadr) maxadr=abadrs;
				bin[(int)abadrs] = ch2num(lbuf[po],lbuf[po+1]);/*Write data to buffer*/
				abadrs++;
			}
		}
	}

	return maxadr+1;
}



int BinToHex(unsigned char *bin, int fsize, std::string &hex)	//static
{
	char lbuf[256];
	int inp, ch,cnt,csum,ofsa;
	long int fpoint,fsub,adrs;

	hex = "";

	cnt = 0; ofsa = 0; adrs = 0; fpoint = 0; inp = 0;
	hex.append(":020000020000FC\n");/*Start Header*/
	fsub = fsize - fpoint;
	if (fsub > 0x20) {
		sprintf(lbuf, ":20%04X00",adrs);	/*Hex line Header*/
  		hex.append(lbuf);
		csum = 0x20 + (adrs>>8) + (adrs & 0xFF);
		adrs = 0x20;
	}
	else {
		sprintf(lbuf, ":%02X%04X00", fsub,adrs);
  		hex.append(lbuf);/*Hex line Header*/
		csum = fsub + (adrs>>8) + (adrs & 0xFF);
		adrs = fsub;
	}
	while (fsub > 0){
		ch = (unsigned char)bin[inp++];
		sprintf(lbuf, "%02X",ch);
		hex.append(lbuf);/*Put data*/
		cnt++; fpoint++;
		fsub = fsize - fpoint;
		csum = ch + csum;
		if((fsub == 0)||(cnt == 0x20)){
			cnt = 0; csum = 0xFF & (~csum + 1);
			sprintf(lbuf, "%02X\n",csum);
			hex.append(lbuf);/*Put checksum*/
			if(fsub == 0) break;
			if(adrs > 0xFFFF){
				ofsa = 0x1000 + ofsa;
				adrs = 0;
				sprintf(lbuf,":02000002%04X",ofsa);
				hex.append(lbuf);/*Change offset address*/
				csum = 0x02 + 0x02 + (ofsa>>8) + (ofsa & 0xFF);
				csum = 0xFF & (~csum + 1);
				sprintf(lbuf,"%02X\n", csum);
				hex.append(lbuf);
			}
			adrs = 0xFFFF & adrs;
			if (fsub > 0x20) {
				sprintf(lbuf,":20%04X00",adrs);
  				hex.append(lbuf);/*Next Hex line Header*/
    			csum = 0x20 + (adrs>>8) + (adrs & 0xFF);
				adrs = adrs + 0x20;
			}
			else {
      			if(fsub > 0){
					sprintf(lbuf,":%02X%04X00", fsub,adrs);
  					hex.append(lbuf);/*Next Hex line Header*/
    				csum = fsub + (adrs>>8) + (adrs & 0xFF);
        			adrs = adrs + fsub;
				}
			}
		}
	}
	sprintf(lbuf,":00000001FF\n");
	hex.append(lbuf);/*End footer*/

	return 1;
}






//IntelHex形式のファイルを読み、バイナリ配列に変換する
//返値:データのバイト数
int loadIntelHex(char *file, unsigned char* image, int maxsize)
{
	FILE *fp;
    std::stringstream ss;
	std::string hex;
	int c;

	//ファイルを読む
	fp = fopen(file, "rb");
	if(!fp){
		return -1;
	}
	while( (c = fgetc(fp)) != EOF ){
		ss << (char)c;
	}
	ss << '\0';
	fclose(fp);
	hex = ss.str();

	//変換
	memset(image, 0, maxsize);
	return (int)HexToBin(hex, image, maxsize);
}


//バイナリ配列をIntelHex形式で保存する
int saveIntelHex(char *file, unsigned char* image, int size)
{
	std::string hex;
	FILE *fp;

	fp = fopen(file, "wb");
	if(!fp) return -1;

	BinToHex(image, size, hex);
	fwrite(hex.c_str(), hex.size(), 1, fp);

	fclose(fp);

	return (int)hex.size();
}


