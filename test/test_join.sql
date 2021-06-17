drop table student_info;
drop table student_grades;

create table student_info
(
    id int unique,
    name varchar(32),
    degree varchar(32),
    primary key(id)
);

create table student_grades
(
    id int unique,
    name varchar(32),
    score float,
    grade varchar(5),
    primary key(id)
);

insert into student_info values (1, "Haotian Xue", "MS");
insert into student_info values (2, "Zirong Chen", "MS");
insert into student_info values (3, "Tiga", "PhD");
insert into student_info values (4, "Ace", "BE");
insert into student_info values (5, "Taro", "BA");
insert into student_info values (6, "Jack", "MBA");
insert into student_info values (7, "Zoffy", "BS");
insert into student_info values (8, "Leo", "ME");
insert into student_info values (9, "Dyna", "MD");
insert into student_info values (10, "Gaia", "AUD");
insert into student_info values (11, "Agul", "BS");
insert into student_info values (12, "Haotian Xue", "BS");
insert into student_info values (13, "Zirong Chen", "BS");

insert into student_grades values (1, "Haotian Xue", 59.9, "B");
insert into student_grades values (2, "Zirong Chen", 59.8, "B");
insert into student_grades values (3, "Tiga", 100, "A");
insert into student_grades values (4, "Ace", 80, "B");
insert into student_grades values (5, "Taro", 99, "A");
insert into student_grades values (6, "Jack", 98, "A");
insert into student_grades values (7, "Zoffy", 0, "F");
insert into student_grades values (8, "Leo", 100, "A");
insert into student_grades values (9, "Dyna", 60, "C");
insert into student_grades values (10, "Gaia", 70, "B");
insert into student_grades values (11, "Agul", 10, "F");
insert into student_grades values (14, "Haotian Xue", 100, "A");
insert into student_grades values (15, "Zirong Chen", 100, "A");



select * from student_info INNER JOIN student_grades ON student_info.id = student_grades.id, student_info.name = student_grades.name WHERE student_grades.grade = "A" OR student_info.name = "Haotian Xue";

select DISTINCT student_info.name from student_info JOIN student_grades ON student_info.id = student_grades.id, student_info.name = student_grades.name WHERE student_grades.grade = "A" OR student_info.name = "Haotian Xue";

select * from student_info JOIN student_grades ON student_info.id = student_grades.id, student_info.name = student_grades.name WHERE student_grades.grade = "A" OR student_info.name = "Haotian Xue" ORDER BY student_grades.score DESC;

select student_info.name, student_grades.grade from student_info JOIN student_grades ON student_info.id = student_grades.id, student_info.name = student_grades.name WHERE student_grades.grade = "A" OR student_info.name = "Haotian Xue";
