#include<common\Root.h>
#include<CardinalityEstimation.h>
#define MAX_VALUE 20000000

int act_sample_num;
double auto_analyze_factor = 1.0;
int end;
int size;
int id_cnt = 0;
std::vector<Histogram *> hist(2); // Histogrammes pour les deux colonnes

void CEEngine::insertTuple(const std::vector<int>& tuple) {
    // Fonction modifée
    // Insertion d'un tuple : mise à jour des histogrammes
    end++;
    id_cnt++;
    for (int i = 0; i < 2; ++i) {
        hist[i]->add(tuple[i]); // Ajout des valeurs dans les histogrammes correspondants
    }
}

void CEEngine::deleteTuple(const std::vector<int>& tuple, int tupleId) {
    // Fonction modifée
    // Suppression d'un tuple : mise à jour des histogrammes
    id_cnt--;
    for (int i = 0; i < 2; ++i) {
        hist[i]->remove(tuple[i]); // Suppression des valeurs dans les histogrammes correspondants
    }
}

int CEEngine::query(const std::vector<CompareExpression>& quals) {
    // Fonction modifée
    // Requête pour estimer la cardinalité
    double ans = 1.0;
    for (int i = 0; i < quals.size(); ++i) {
        CompareExpression expr = quals[i];
        if (expr.compareOp == GREATER) {
            ans *= hist[expr.columnIdx]->getCountBetweenValues(expr.value, MAX_VALUE);
        } else { // Cas EQUAL
            ans *= hist[expr.columnIdx]->getCountAtValue(expr.value);
        }
        ans /= act_sample_num; // Ajustement basé sur l'échantillonnage
    }
    ans *= size; // Mise à l'échelle sur l'ensemble des données
    return static_cast<int>(std::round(ans)); // Retourne une estimation entière
}

void CEEngine::prepare() {
    // Fonction modifée
    // Préparation des histogrammes si nécessaire
    if (std::abs(id_cnt) * 1.0 / size >= auto_analyze_factor) {
        sample(size, 10000, hist, dataExecuter); // Ré-échantillonnage
        size += id_cnt; // Mise à jour de la taille effective
        id_cnt = 0; // Réinitialisation
    }
}

CEEngine::CEEngine(int num, DataExecuter *dataExecuter) {
    // Fonction modifée
    // Constructeur : initialisation des variables et des histogrammes
    this->dataExecuter = dataExecuter;
    size = num;
    end = num;

    hist[0] = new Histogram(3000, MAX_VALUE + 1, 0); // Histogramme pour la colonne A
    hist[1] = new Histogram(3000, MAX_VALUE + 1, 0); // Histogramme pour la colonne B

    sample(size, 10000, hist, dataExecuter); // Initialisation avec un échantillon
}

void CEEngine::sample(int end, int size, std::vector<Histogram *> &hist, DataExecuter *dataExecuter) {
    // Fonction modifée
    // Fonction d'échantillonnage pour construire les histogrammes
    std::vector<std::vector<int>> samp;
    int start = rand() % (end - size); // Choisir un point de départ aléatoire
    dataExecuter->readTuples(start, size, samp); // Lire les tuples

    if (samp.empty()) return; // Rien à faire si pas de données

    act_sample_num = samp.size(); // Nombre d'échantillons effectifs

    for (int i = 0; i < 2; ++i) {
        delete hist[i];
        hist[i] = new Histogram(3000, MAX_VALUE + 1, 0); // Réinitialisation des histogrammes
    }
    for (const auto& tuple : samp) {
        for (int j = 0; j < 2; ++j) {
            hist[j]->add(tuple[j]); // Ajout des données dans les histogrammes
        }
    }
    hist[0]->build(); // Construire l'histogramme pour A
    hist[1]->build(); // Construire l'histogramme pour B
}
