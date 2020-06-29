use lin2world
go

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[Pledge_War]') AND type in (N'U'))
BEGIN
CREATE TABLE [dbo].[Pledge_War](
	[id] [int] IDENTITY(1,1) NOT NULL,
	[status] [int] NOT NULL,
	[challenger] [int] NOT NULL,
	[challengee] [int] NOT NULL,
	[challenger_name] VARCHAR(50),
	[challengee_name] VARCHAR(50),
	[begin_time] [int] NOT NULL,
	[end_time] [int] NOT NULL,
	[winner] [int] NOT NULL,
	[winner_name] VARCHAR(50)
 CONSTRAINT [PK_Pledge_War] PRIMARY KEY CLUSTERED 
(
	[id] ASC
)WITH (IGNORE_DUP_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
END
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[Alliance_War]') AND type in (N'U'))
BEGIN
CREATE TABLE [dbo].[Alliance_War](
	[id] [int] IDENTITY(1,1) NOT NULL,
	[status] [int] NOT NULL,
	[challenger] [int] NOT NULL,
	[challengee] [int] NOT NULL,
	[challenger_name] VARCHAR(50),
	[challengee_name] VARCHAR(50),
	[begin_time] [int] NOT NULL,
	[end_time] [int] NOT NULL,
	[winner] [int] NOT NULL,
	[winner_name] VARCHAR(50)
 CONSTRAINT [PK_Alliance_War] PRIMARY KEY CLUSTERED 
(
	[id] ASC
)WITH (IGNORE_DUP_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
END
GO