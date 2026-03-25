/* Fichier : fakesdb.h
   base de donnees pour la detection naive de fake news */

#ifndef FAKESDB_H
#define FAKESDB_H

#define NB_FAKES    6
#define NB_SUSPECTS 8

static const char *BASE_FAKES[] = {
    "ils vous cachent",
    "partagez avant suppression",
    "ce qu on vous cache",
    "la verite sur",
    "les medias ne disent pas",
    "wake up"
};

static const char *MOTS_SUSPECTS[] = {
    "alerte",
    "urgent",
    "exclusif",
    "censure",
    "complot",
    "secret",
    "interdit",
    "choc"
};

#endif
