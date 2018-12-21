//
// Created by roman on 18.12.18.
//

#ifndef TRIGRAMS_CONT_TRIGRAMSCONTAINER_H
#define TRIGRAMS_CONT_TRIGRAMSCONTAINER_H


#include <QtCore/QVector>

using trigram = uint32_t;

class TrigramsContainer {
public:
    TrigramsContainer():trigrams(QVector<trigram>{}){};
    void insert(trigram);
    void makeAccessible();
    bool contains(trigram);
    QVector<trigram>::iterator begin();
    QVector<trigram>::iterator end();
    int size();
    void clear();
private:
    QVector<trigram> trigrams;
};


#endif //TRIGRAMS_CONT_TRIGRAMSCONTAINER_H
