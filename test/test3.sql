drop table student;

create table student
(
    id int unique,
    name varchar(32),
    score float,
    primary key(id)
);

insert into student
    values (1, "Haotian Xue", 59.9);

select * from student;
