drop table student;

create table student
(
    id int unique,
    name varchar(32),
    score float,
    primary key(id)
);

insert into student
values (5, "Haotian Xue", 90);
insert into student
values (1, "Haotian Xue", 59.9);
insert into student
values (3, "Tiga", 100);
insert into student
values (4, "HaHa", 120);
insert into student
values (2, "ZiRong Chen", 59.9);

select MAX(score) from student;

select MAX(name) from student;