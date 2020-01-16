INSERT INTO Verbs VALUES ( ?i ?t ?t1 ?t2 ) ;
SELECT COUNT(*) FROM Verbs ;
SELECT * FROM Verbs ORDER BY infinitive ;
SELECT * FROM Verbs WHERE (instr(infinitive, ?s) > 0) OR (instr(translation, ?s) > 0) OR (instr(time1, ?s) > 0) OR (instr(time2, ?s) > 0) ORDER BY infinitive ;
CREATE TABLE Verbs (
    infinitive TEXT PRIMARY KEY,
    translation TEXT,
    time1 TEXT,
    time2 TEXT
) ;
