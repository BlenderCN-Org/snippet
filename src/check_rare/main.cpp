/*
�l�^�́H
catch_rare�ɂ���
-> ���ʂɕ��U�����ƌ��o�͂�������?
-> �ǂ̂悤�ɓ��삷�邩�H
-> ture,false,false,false,false�ŏ��߂Ĕ��������肷��́H
-> CLT���g���Ĕ��肷��̂ł͂Ȃ��A�������m���Ɋւ��Ă̓|���\�����z�̂ق����K���Ă���
-> ���p��Ƃ��āA���ނɂ��g����B�ǂ̃p�X��I�������̂��݂����ȁH
-> ���p��: �{���ɔ����ŋN���邩(�R�C���g�X�A��������)
-> ���p��: ���z�̊����̎Z�o

�Q��
https://pharr.org/matt/blog/2018/05/31/check-rare.html

TODOs
- �������쐬����
- �����ƌ��o���Ă���̂��m�F����
- TODO: �T���v���v���O�������쐬����
- TODO: �v���O�����������Ȃ����{�I�Ȍ����𗝉�����
- TODO: ���p��Ȃǂ�����Ă݂�
- TODO: �T���v�������S�ɓ������猴���������Ă݂�
- TODO: �����N����Ȃ����Ƃ̋�Ԑ����1/N
- TODO: ���o����܂ł̃J�E���g�ő���
*/

#include <cstdint>
#include <cstdio>
#include <random>

// true/false�𐶐�����
class Generator
{
public:
    Generator(float freq)
        :freq_(freq)
    {}
    bool gen()
    {
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        return (dist(rng_) < freq_);
    }
private:
    std::mt19937 rng_;
    float freq_ = 0.0f;
};

// true,false�̊m�F
class Checker
{
public:
    //
    Checker(float maxFreq)
        :maxFreq_(maxFreq)
    {}
    //
    bool add(bool isTrue)
    {
        ++total_;
        numTrue_ += (isTrue ? 1 : 0);
        //
        const float freqEstimate = float(numTrue_) / float(total_);
        const float varEst = freqEstimate * (1 - freqEstimate) / float(total_);
        //
        return
            (freqEstimate - sigma_ * varEst >= maxFreq_);
    }
private:
    float maxFreq_ = 0.0f;
    float sigma_ = 2.0f;
    int32_t total_ = 0;
    int32_t numTrue_ = 0;
};

int32_t main()
{
    // 
    Generator generator(0.05f);
    Checker checker(0.05f);
    for (int32_t i=0;i<128;++i)
    {
        if (checker.add(generator.gen()))
        {
            printf("find\n");
            break;
        }
    }
}