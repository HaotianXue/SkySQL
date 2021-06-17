drop table student;

create table student
(
    id int unique,
    name varchar(32),
    score float,
    primary key(id)
);

create table student
(
    id int unique,
    name varchar(32),
    score float,
    primary key(id)
);

create index name_index on student (name);
create index name_index on student (name);
drop index name_index;
drop index name_index;

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


select * from student ORDER BY score DESC;
select * from student ORDER BY name ASC, score DESC;
select * from student ORDER BY id , name , score;
select * from student ORDER BY name , score , id;
select * from student ORDER BY score , name , id;
